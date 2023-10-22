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
#include "sql/stmt/groupby_stmt.h"
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

  // 1. 先处理 from clause 收集表信息
  // from 中的 table 有两个层级 第一级是笛卡尔积 第二级是 INNER JOIN
  // e.g. (t1 inner join t2 inner join t3, t4) -> (t1, t2, t3), (t4)
  // 收集表信息的同时这里做了 predicate expr 下推
  // 处理结果保存在 join_tables 中
  std::vector<Table *> tables; // 收集所有 table
  std::unordered_map<std::string, Table *> table_map; // 收集所有 table
  std::unordered_map<std::string, Table *> local_table_map; // 每次收集第二级的 table
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

  // 先收集所有的 condition 用于后续下推
  // conditions: t1 inner join t2 on xxx and xxx inner join t3 on xxx and xxx
  // NOTE: 这里并没有 clear 原来的 condition 信息，后续要进行语义检查
  std::vector<ConditionSqlNode>& all_conditions = const_cast<std::vector<ConditionSqlNode>&>(select_sql.conditions);
  for (size_t i = 0; i < select_sql.relations.size(); ++i) {
    const InnerJoinSqlNode& relations = select_sql.relations[i];
    const std::vector<std::vector<ConditionSqlNode>>& conditions = relations.conditions;
    for (auto& on_conds : conditions) {
      all_conditions.insert(all_conditions.end(), on_conds.begin(), on_conds.end());
      // on_conds.clear();
    }
    // conditions.clear();
  }

  // 判断表达式是否可以下推
  auto check_can_push_down = [&local_table_map](const Expression* expr) -> RC {
    if (expr->type() == ExprType::AGGRFUNCTION) {
      // aggr func expr do not push down
      return RC::INTERNAL;
    }
    if (expr->type() == ExprType::FIELD) {
      const FieldExpr* field_expr = static_cast<const FieldExpr*>(expr);
      if (0 != local_table_map.count(field_expr->get_table_name())) {
        return RC::SUCCESS;
      }
      return RC::INTERNAL;
    }
    return RC::SUCCESS;
  };

  // 判断 condition 是否可以下推
  auto cond_is_ok = [&check_can_push_down, &local_table_map](const ConditionSqlNode& node) ->bool {
    return RC::SUCCESS == node.left_expr->traverse_check(check_can_push_down) &&
            RC::SUCCESS == node.right_expr->traverse_check(check_can_push_down);
  };

  // 从 all_conditions 中挑选出可以下推的 conditions
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

  // t1 inner join t2 inner join t3 -> process t1 -> process t2 -> process t3
  auto process_one_relation = [&](const std::string& relation, JoinTables& jt) {
    RC rc = RC::SUCCESS;
    // check and collect table to tables table_map local_table_map
    if (rc = check_and_collect_tables(relation.c_str()); rc != RC::SUCCESS) {
      return rc;
    }

    // get push-down-ok conditions and create filterstmt
    // 直接在这里完成表达式的下推，优化阶段下推有点子麻烦
    auto ok_conds = pick_conditions();
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

  // xxx from (t1 inner join t2), (t3), (t4 inner join t5) where xxx
  for (size_t i = 0; i < select_sql.relations.size(); ++i) {
    // t1 inner join t2 on xxx inner join t3 on xxx
    InnerJoinSqlNode& relations = select_sql.relations[i]; // why not const : will clear its conditions
    local_table_map.clear();

    // construct JoinTables
    JoinTables jt;

    // base relation: **t1** inner join t2 on xxx inner join t3 on xxx
    RC rc = process_one_relation(relations.base_relation, jt);
    if (RC::SUCCESS != rc) {
      return rc;
    }

    // join relations: t1 inner join **t2** on xxx inner join **t3** on xxx
    const std::vector<std::string>& join_relations = relations.join_relations;
    std::vector<std::vector<ConditionSqlNode>>& conditions = relations.conditions;
    for (size_t j = 0; j < join_relations.size(); ++j) {
      // check table and push down
      if (RC::SUCCESS != (rc = process_one_relation(join_relations[j], jt))) {
        return rc;
      }
      // check source conditions
      // 如果原来的 condition 能下推那么就一定是符合语义的
      for (auto& condition : conditions[j]) {
        if (!cond_is_ok(condition)) {
          return rc;
        }
      }
      conditions[i].clear();
    }

    // push jt to join_tables
    join_tables.emplace_back(std::move(jt));
  }

  // 2. collect query exprs in `select` statement
  // 语义解析 check & 设置表达式 name 用于客户端输出的表头
  // 要处理 *, *.*, t1.* 这种情况
  // 要 check field 判断**所有** FieldExpr 是否合法：有一个唯一对应的 table 即合法 不能没有表 也不能出现歧义
  bool is_single_table = (tables.size() == 1);
  std::vector<std::unique_ptr<Expression>> projects;
  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
  }

  // 检查所有的 FieldExpr 是否合法 并且判断一下是否有 AggrFuncExpr
  bool has_aggr_func_expr = false;
  auto check_field = [&table_map, &tables, &db, &default_table, &has_aggr_func_expr](Expression *expr) {
    if (expr->type() == ExprType::AGGRFUNCTION) {
      has_aggr_func_expr = true;
    }
    if (expr->type() == ExprType::FIELD) {
      FieldExpr* field_expr = static_cast<FieldExpr*>(expr);
      return field_expr->check_field(table_map, tables, db, default_table);
    }
    return RC::SUCCESS;
  };

  for (int i = static_cast<int>(select_sql.project_exprs.size()) - 1; i >= 0; i--) {
    RC rc = RC::SUCCESS;
    Expression* expr = select_sql.project_exprs[i]; // 将 sqlNode 的表达式转移到 SelectStmt 中
    // 单独处理 select 后跟 * 的情况 select *; select *.*; select t1.*
    if (expr->type() == ExprType::FIELD) {
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
        if(rc = field_expr->check_field(table_map, tables, db, default_table); rc != RC::SUCCESS) {
          LOG_INFO("expr->check_field error!");
          return rc;
        }
        projects.emplace_back(expr);
      }
    } else {
      if (rc = expr->traverse_check(check_field); rc != RC::SUCCESS) {
        LOG_INFO("project expr traverse check_field error!");
        return rc;
      }
      projects.emplace_back(expr);
    }
  }
  select_sql.project_exprs.clear();

  LOG_INFO("got %d tables in from stmt and %d exprs in query stmt", tables.size(), projects.size());

  RC rc = RC::SUCCESS;
  // create filter statement in `where` statement
  // NOTE: 这里 all_conditions 可能是空的
  FilterStmt *filter_stmt = nullptr;
  if (!all_conditions.empty()) {
    rc = FilterStmt::create(db,
        default_table,
        &table_map,
        all_conditions.data(),
        static_cast<int>(all_conditions.size()),
        filter_stmt);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct filter stmt");
      return rc;
    }
  }

  // TODO 还未在语法层面支持 group by clause
  GroupByStmt *groupby_stmt = nullptr;
  // 有聚集函数表达式 或者有 group by clause 就要添加 group by stmt
  if (has_aggr_func_expr /* || nullptr != select_sql.groupby_ */) {
    // 1. 提取 AggrFuncExpr 以及不在 AggrFuncExpr 中的 FieldExpr
    std::vector<std::unique_ptr<AggrFuncExpr>> aggr_exprs;
    std::vector<std::unique_ptr<FieldExpr>> field_exprs_not_in_aggr;
    // 用于从 project exprs 中提取所有 aggr func exprs. e.g. min(c1 + 1) + 1
    auto collect_aggr_exprs = [&aggr_exprs](Expression * expr) {
      if (expr->type() == ExprType::AGGRFUNCTION) {
        aggr_exprs.emplace_back(static_cast<AggrFuncExpr*>(static_cast<AggrFuncExpr*>(expr)->deep_copy().release()));
      }
    };
    // 用于从 project exprs 中提取所有不在 aggr func expr 中的 field expr
    auto collect_field_exprs = [&field_exprs_not_in_aggr](Expression * expr) {
      if (expr->type() == ExprType::FIELD) {
        field_exprs_not_in_aggr.emplace_back(static_cast<FieldExpr*>(static_cast<FieldExpr*>(expr)->deep_copy().release()));
      }
    };
    // do extract
    for (auto& project : projects) {
      project->traverse(collect_aggr_exprs);
      project->traverse(collect_field_exprs, [](const Expression* expr) { return expr->type() != ExprType::AGGRFUNCTION; });
    }

    // 2. 语义检查 check:
    // - 聚集函数参数个数、参数为 * 的检查是在 syntax parser 完成
    // - 聚集函数中的字段 OK
    // - 非聚集函数中的字段应该 必须在 group by 的字段中
    // - 没有 group by clause 时，不应该有非聚集函数中的字段
    // 当前没有 group by，所以只 check 一下有没有不在聚集函数中的字段即可
    if (!field_exprs_not_in_aggr.empty()) {
      LOG_ERROR("No Group By. But Has Fields Not In Aggr Func");
      return RC::INVALID_ARGUMENT;
    }
    
    // 3. create groupby stmt
    rc = GroupByStmt::create(db,
        default_table,
        &table_map,
        nullptr, //暂时先传入 nullptr
        groupby_stmt,
        std::move(aggr_exprs),
        std::move(field_exprs_not_in_aggr));
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct groupby stmt");
      return rc;
    }
  }

  // everything alright
  // NOTE: 此时 select_sql 原有的部分信息已被移除 后续不得使用
  SelectStmt *select_stmt = new SelectStmt();
  // TODO add expression copy
  select_stmt->join_tables_.swap(join_tables);
  select_stmt->projects_.swap(projects);
  select_stmt->filter_stmt_ = filter_stmt; // maybe nullptr
  select_stmt->groupby_stmt_ = groupby_stmt; // maybe nullptr
  stmt = select_stmt;
  return RC::SUCCESS;
}
