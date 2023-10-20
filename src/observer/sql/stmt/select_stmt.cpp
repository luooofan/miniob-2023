/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

static void wildcard_fields(const Table *table, std::vector<std::unique_ptr<Expression>> &projects, bool is_single_table)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    auto field = table_meta.field(i);
    if (field->visible()) {
      FieldExpr *tmp = new FieldExpr(table, field);
      if(is_single_table) {
        tmp->set_name(tmp->get_field_name()); // should same as origin
      } else {
        tmp->set_name(tmp->get_table_name() + "." + tmp->get_field_name());
      }  
      projects.emplace_back(tmp);
    }
  }
}

RC SelectStmt::create(Db *db, SelectSqlNode &select_sql, Stmt *&stmt)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;
  std::unordered_map<std::string, Table *> local_table_map;
  std::vector<JoinTables> join_tables;
  // collect tables in `from` statement
  auto check_and_collect_tables = [&](const char* table_name) {
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null.");
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_name, table));
    local_table_map.insert(std::pair<std::string, Table *>(table_name, table));
    return RC::SUCCESS;
  };

  // 直接在这里完成表达式的下推，优化阶段下推有点子麻烦
  // 先收集所有的 condition
  std::vector<ConditionSqlNode>& all_conditions = const_cast<std::vector<ConditionSqlNode>&>(select_sql.conditions);
  for (size_t i = 0; i < select_sql.relations.size(); ++i) {
    const InnerJoinSqlNode& relations = select_sql.relations[i];
    std::vector<std::vector<ConditionSqlNode>>& conditions = const_cast<decltype(relations.conditions)&>(relations.conditions);
    for (auto& on_conds : conditions) {
      all_conditions.insert(all_conditions.end(), on_conds.begin(), on_conds.end());
      on_conds.clear();
    }
    conditions.clear();
  }

  auto cond_is_ok = [&local_table_map](const ConditionSqlNode& node) {
    return node.left_expr->check_can_push_down(local_table_map) 
            && node.right_expr->check_can_push_down(local_table_map);
  };

  auto pick_conditions = [&cond_is_ok, &all_conditions]() {
    std::vector<ConditionSqlNode> res;
    for (auto iter = all_conditions.begin(); iter != all_conditions.end(); ) {
      if (cond_is_ok(*iter)) {
        res.emplace_back(*iter);
        iter = all_conditions.erase(iter);
      } else {
        iter++;
      }
    }
    return res;
  };

  auto process_one_relation = [&](const std::string& relation, JoinTables& jt) {
    RC rc = RC::SUCCESS;
    if (rc = check_and_collect_tables(relation.c_str()); rc != RC::SUCCESS) {
      return rc;
    }
    auto ok_conds = pick_conditions();
    // create FilterStmt
    FilterStmt* filter_stmt = nullptr;
    if (!ok_conds.empty()) {
      if (rc = FilterStmt::create(db, table_map[relation], &table_map, ok_conds.data(), ok_conds.size(), filter_stmt);
              rc != RC::SUCCESS) {
        return rc;
      }
      ASSERT(nullptr != filter_stmt, "FilterStmt is null!");
    }
    // fill JoinTables
    jt.push_join_table(table_map[relation], filter_stmt);
    return rc;
  };

  for (size_t i = 0; i < select_sql.relations.size(); ++i) {
    const InnerJoinSqlNode& relations = select_sql.relations[i];
    local_table_map.clear();

    // construct JoinTables
    JoinTables jt;

    // base relation
    RC rc = process_one_relation(relations.base_relation, jt);
    if (RC::SUCCESS != rc) {
      return rc;
    }

    const std::vector<std::string>& join_relations = relations.join_relations;
    for (size_t j = 0; j < join_relations.size(); ++j) {
      if (RC::SUCCESS != (rc = process_one_relation(join_relations[j], jt))) {
        return rc;
      }
    }

    // push jt to join_tables
    join_tables.emplace_back(std::move(jt));
  }

  // collect query exprs in `select` statement
  // set exprs name
  bool is_single_table = (tables.size() == 1);
  std::vector<std::unique_ptr<Expression>> projects;
  for (int i = static_cast<int>(select_sql.project_exprs.size()) - 1; i >= 0; i--) {
    RC rc = RC::SUCCESS;
    Expression* expr = select_sql.project_exprs[i]; //将sqlNode的表达式转移到SelectStmt中
    // 单独处理 select 后跟 * 的情况 select *; select *.*; select t1.*
    if(expr->type() == ExprType::FIELD) {
      FieldExpr *field_expr = static_cast<FieldExpr*>(expr);
      const char *table_name = field_expr->get_table_name().c_str();
      const char *field_name = field_expr->get_field_name().c_str();
      ASSERT(!common::is_blank(field_name), "Parse ERROR!");
      if ((0 == strcmp(table_name, "*")) && (0 == strcmp(field_name, "*"))) { // * or *.*
        for (const Table *table : tables) {
          wildcard_fields(table, projects, is_single_table);
        }
      } else if(0 == strcmp(field_name, "*")) { // t1.*
        ASSERT(0 != strcmp(table_name, "*"), "Parse ERROR!");
        auto iter = table_map.find(table_name);
        if (iter == table_map.end()) {
          LOG_WARN("no such table in from list: %s", table_name);
          return RC::SCHEMA_FIELD_MISSING;
        }

        wildcard_fields(iter->second, projects, is_single_table);
      } else { // t1.c1 or c1
        if(rc = expr->check_field(table_map, tables, db); rc != RC::SUCCESS) {
          LOG_INFO("expr->check_field error!");
          return rc;
        }
        projects.emplace_back(expr);
      }
    } else {
      if(rc = expr->check_field(table_map,tables,db); rc != RC::SUCCESS) {
        LOG_INFO("expr->check_field error!");
        return rc;
      }
      projects.emplace_back(expr);
    }
  }//end for
  select_sql.project_exprs.clear();
  LOG_INFO("got %d tables in from stmt and %d exprs in query stmt", tables.size(), projects.size());

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
  }

  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db,
      default_table,
      &table_map,
      all_conditions.data(),
      static_cast<int>(all_conditions.size()),
      filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // everything alright
  SelectStmt *select_stmt = new SelectStmt();
  // TODO add expression copy
  select_stmt->join_tables_.swap(join_tables);
  select_stmt->projects_.swap(projects);
  select_stmt->filter_stmt_ = filter_stmt;
  stmt = select_stmt;
  return RC::SUCCESS;
}
