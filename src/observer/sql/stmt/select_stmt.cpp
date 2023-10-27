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
#include "sql/stmt/orderby_stmt.h"
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
  if (nullptr != groupby_stmt_) {
    delete groupby_stmt_;
    groupby_stmt_ = nullptr;
  }
  if (nullptr != orderby_stmt_) {
    delete orderby_stmt_;
    orderby_stmt_ = nullptr;
  }
  if (nullptr != having_stmt_) {
    delete having_stmt_;
    having_stmt_ = nullptr;
  }
}

static void wildcard_fields(const Table *table, const std::string& alias, std::vector<std::unique_ptr<Expression>> &projects, bool is_single_table)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    auto field = table_meta.field(i);
    if (field->visible()) {
      FieldExpr *tmp = new FieldExpr(table, field);
      if (is_single_table) {
        tmp->set_name(tmp->get_field_name()); // should same as origin
      } else if (alias.empty()) {
        tmp->set_name(tmp->get_table_name() + "." + tmp->get_field_name());
      } else {
        tmp->set_name(alias + "." + tmp->get_field_name());
      }
      projects.emplace_back(tmp);
    }
  }
}

RC SelectStmt::process_from_clause(Db *db, std::vector<Table *> &tables,
    std::unordered_map<std::string, std::string> &table_alias_map,
    std::unordered_map<std::string, Table *> &table_map,
    std::vector<InnerJoinSqlNode> &from_relations,
    std::vector<JoinTables> &join_tables)
{
  std::unordered_set<std::string> table_alias_set; // 检测别名是否有重复

  // collect tables info in `from` statement
  auto check_and_collect_tables = [&](const std::pair<std::string, std::string>& table_name_pair) {
    const std::string& src_name = table_name_pair.first;
    const std::string& alias = table_name_pair.second;
    if (src_name.empty()) {
      LOG_WARN("invalid argument. relation name is null.");
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(src_name.c_str());
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), src_name.c_str());
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(src_name, table));
    if (!alias.empty()) {
      // 需要考虑别名重复的问题
      // NOTE: 这里不能用 table_map 因为其中有 parent table
      if (table_alias_set.count(alias) != 0) {
        return RC::INVALID_ARGUMENT;
      }
      table_alias_set.insert(alias);
      table_alias_map.insert(std::pair<std::string, std::string>(src_name, alias));
      table_map.insert(std::pair<std::string, Table *>(alias, table));
    }
    return RC::SUCCESS;
  };

  // t1 inner join t2 inner join t3 -> process t1 -> process t2 -> process t3
  auto process_one_relation = [&](const std::pair<std::string, std::string>& relation, JoinTables& jt, Expression* condition) {
    RC rc = RC::SUCCESS;
    // check and collect table to tables table_map local_table_map
    if (rc = check_and_collect_tables(relation); rc != RC::SUCCESS) {
      return rc;
    }

    // create filterstmt
    FilterStmt* filter_stmt = nullptr;
    if (condition != nullptr) {
      // TODO 这里重新考虑下父子查询
      // TODO select * from t1 where c1 in (select * from t2 inner join t3 on t1.c1 > 0 inner join t1) ?
      if (rc = FilterStmt::create(db, table_map[relation.first], &table_map, condition, filter_stmt);
              rc != RC::SUCCESS) {
        return rc;
      }
      ASSERT(nullptr != filter_stmt, "FilterStmt is null!");
    }

    // fill JoinTables
    jt.push_join_table(table_map[relation.first], filter_stmt);
    return rc;
  };

  // xxx from (t1 inner join t2), (t3), (t4 inner join t5) where xxx
  for (size_t i = 0; i < from_relations.size(); ++i) {
    // t1 inner join t2 on xxx inner join t3 on xxx
    InnerJoinSqlNode& relations = from_relations[i]; // why not const : will clear its conditions
    // local_table_map = parent_table_map; // from clause 中的 expr 可以使用父查询的表中的字段

    // construct JoinTables
    JoinTables jt;

    // base relation: **t1** inner join t2 on xxx inner join t3 on xxx
    RC rc = process_one_relation(relations.base_relation, jt, nullptr);
    if (RC::SUCCESS != rc) {
      LOG_WARN("Create SelectStmt: From Clause: Process Base Relation %s Failed!", relations.base_relation.first.c_str());
      return rc;
    }

    // join relations: t1 inner join **t2** on xxx inner join **t3** on xxx
    const std::vector<std::pair<std::string, std::string>>& join_relations = relations.join_relations;
    std::vector<Expression*>& conditions = relations.conditions;
    for (size_t j = 0; j < join_relations.size(); ++j) {
      if (RC::SUCCESS != (rc = process_one_relation(join_relations[j], jt, conditions[j]))) {
        return rc;
      }
    }
    conditions.clear(); // 其所有权已经都交给了 FilterStmt

    // push jt to join_tables
    join_tables.emplace_back(std::move(jt));
  }
  return RC::SUCCESS;
}

// parent_table_map 是父查询中的 table_map，这里只需要父查询的 table map 即可
// tables table_alias_map local_table_map 都不需要
// 嵌套子查询 的情况下在 parent table map 中累积 这里不把它维护成 vector
RC SelectStmt::create(Db *db, SelectSqlNode &select_sql, Stmt *&stmt,
    const std::unordered_map<std::string, Table *> &parent_table_map)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  // 1. 先处理 from clause 收集表信息
  // from 中的 table 有两个层级 第一级是笛卡尔积 第二级是 INNER JOIN
  // e.g. (t1 inner join t2 inner join t3, t4) -> (t1, t2, t3), (t4)
  std::vector<Table *> tables; // 收集所有 table 主要用于解析 select *
  std::unordered_map<std::string, std::string> table_alias_map; // <table src name, table alias name>
  std::unordered_map<std::string, Table *> table_map = parent_table_map; // 收集所有 table 包括所有祖先查询的 table
  std::vector<JoinTables> join_tables;
  RC rc = process_from_clause(db, tables, table_alias_map, table_map, select_sql.relations, join_tables);
  if (OB_FAIL(rc)) {
    LOG_WARN("SelectStmt-FromClause: Process Failed! RETURN %d", rc);
    return rc;
  }

  // 2. collect query exprs in `select` statement
  // 语义解析 check & 设置表达式 name alias 用于客户端输出的表头
  // 要处理 *, *.*, t1.* 这种情况
  // 要 check field 判断**所有** FieldExpr 是否合法：有一个唯一对应的 table 即合法 不能没有表 也不能出现歧义
  std::vector<std::unique_ptr<Expression>> projects;
  Table *default_table = nullptr;
  bool is_single_table = (tables.size() == 1);
  if (is_single_table) {
    default_table = tables[0];
  }

  // 检查所有的 FieldExpr 和 SysFuncExpr 是否合法 并且判断一下是否有 AggrFuncExpr
  // projects 中不会出现 subquery
  bool has_aggr_func_expr = false;
  auto check_project_expr = [&table_map, &tables, &default_table, &table_alias_map, &has_aggr_func_expr](Expression *expr) {
    if (expr->type() == ExprType::SUBQUERY) {
      return RC::INVALID_ARGUMENT;
    }
    if (expr->type() == ExprType::SYSFUNCTION) {
      SysFuncExpr* sysfunc_expr = static_cast<SysFuncExpr*>(expr);
      return sysfunc_expr->check_param_type_and_number();
    }
    if (expr->type() == ExprType::FIELD) {
      FieldExpr* field_expr = static_cast<FieldExpr*>(expr);
      return field_expr->check_field(table_map, tables, default_table, table_alias_map);
    }
    if (expr->type() == ExprType::AGGRFUNCTION) {
      has_aggr_func_expr = true;
    }
    return RC::SUCCESS;
  };

  for (size_t i = 0; i < select_sql.project_exprs.size(); ++i) {
    Expression* expr = select_sql.project_exprs[i];
    // 单独处理 select 后跟 * 的情况 select *; select *.*; select t1.*
    if (expr->type() == ExprType::FIELD) {
      FieldExpr *field_expr = static_cast<FieldExpr*>(expr);
      const char *table_name = field_expr->get_table_name().c_str();
      const char *field_name = field_expr->get_field_name().c_str();
      ASSERT(!common::is_blank(field_name), "Parse ERROR!");
      if ((0 == strcmp(table_name, "*")) && (0 == strcmp(field_name, "*"))) { // * or *.*
        if (tables.empty() || !field_expr->alias().empty()) {
          return RC::INVALID_ARGUMENT; // not allow: select *; select * as xxx;
        }
        for (const Table *table : tables) {
          if (table_alias_map.count(table->name())) {
            wildcard_fields(table, table_alias_map[table->name()], projects, is_single_table);
          } else {
            wildcard_fields(table, std::string(), projects, is_single_table);
          }
        }
      } else if(0 == strcmp(field_name, "*")) { // t1.*
        ASSERT(0 != strcmp(table_name, "*"), "Parse ERROR!");
        auto iter = table_map.find(table_name);
        if (iter == table_map.end()) {
          LOG_WARN("no such table in from list: %s", table_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
        Table* table = iter->second;
        if (table_alias_map.count(table->name())) {
          wildcard_fields(table, table_alias_map[table->name()], projects, is_single_table);
        } else {
          wildcard_fields(table, std::string(), projects, is_single_table);
        }
      } else { // t1.c1 or c1
        if(rc = field_expr->check_field(table_map, tables, default_table, table_alias_map); rc != RC::SUCCESS) {
          LOG_INFO("expr->check_field error!");
          return rc;
        }
        projects.emplace_back(expr);
      }
    } else {
      if (rc = expr->traverse_check(check_project_expr); rc != RC::SUCCESS) {
        LOG_WARN("project expr traverse check_field error!");
        return rc;
      }
      projects.emplace_back(expr);
    }
  }
  select_sql.project_exprs.clear(); // 管理权已经移交到 projects 中 后续会交给 select stmt

  LOG_INFO("got %d tables in from clause and %d exprs in query clause", tables.size(), projects.size());

  FilterStmt *filter_stmt = nullptr; // TODO release memory when failed
  if (select_sql.conditions != nullptr) {
    rc = FilterStmt::create(db,
        default_table,
        &table_map,
        select_sql.conditions,
        filter_stmt);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct filter stmt");
      return rc;
    }
  }

  GroupByStmt *groupby_stmt = nullptr; // TODO release memory when failed
  FilterStmt *having_filter_stmt = nullptr; // TODO release memory when failed
  // 有聚集函数表达式 或者有 group by clause 就要添加 group by stmt
  if (has_aggr_func_expr || select_sql.groupby_exprs.size() > 0) {
    // 1. 提取 AggrFuncExpr 以及不在 AggrFuncExpr 中的 FieldExpr
    std::vector<std::unique_ptr<AggrFuncExpr>> aggr_exprs;
    //select 子句中出现的所有 fieldexpr 都需要传递收集起来,
    std::vector<std::unique_ptr<FieldExpr>> field_exprs;//这个 vector 需要传递给 order by 算子
    std::vector<std::unique_ptr<Expression>> field_exprs_not_aggr; //select 后的所有非 aggrexpr 的 field_expr,用来判断语句是否合法 
    // 用于从 project exprs 中提取所有 aggr func exprs. e.g. min(c1 + 1) + 1
    auto collect_aggr_exprs = [&aggr_exprs](Expression * expr) {
      if (expr->type() == ExprType::AGGRFUNCTION) {
        aggr_exprs.emplace_back(static_cast<AggrFuncExpr*>(static_cast<AggrFuncExpr*>(expr)->deep_copy().release()));
      }
    };
    // 用于从 project exprs 中提取所有field expr,
    auto collect_field_exprs = [&field_exprs](Expression * expr) {
      if (expr->type() == ExprType::FIELD) {
        field_exprs.emplace_back(static_cast<FieldExpr*>(static_cast<FieldExpr*>(expr)->deep_copy().release()));
      }
    };
    // 用于从 project exprs 中提取所有不在 aggr func expr 中的 field expr
    auto collect_exprs_not_aggexpr = [&field_exprs_not_aggr](Expression * expr) {
        if (expr->type() == ExprType::FIELD) {
        field_exprs_not_aggr.emplace_back(static_cast<FieldExpr*>(static_cast<FieldExpr*>(expr)->deep_copy().release()));
      }
    };
    // do extract
    for (auto& project : projects) {
      project->traverse(collect_aggr_exprs);//提取所有 aggexpr
      project->traverse(collect_field_exprs );//提取 select clause 中的所有 field_expr,传递给groupby stmt
      //project->traverse(collect_field_exprs, [](const Expression* expr) { return expr->type() != ExprType::AGGRFUNCTION; });

      //提取所有不在 aggexpr 中的 field_expr，用于语义检查
      project->traverse(collect_exprs_not_aggexpr,[](const Expression* expr) { return expr->type() != ExprType::AGGRFUNCTION; });
    }
    //针对 having 后的表达式，需要做和上面相同的三个提取过程
    // select id, max(score) from t_group_by group by id having count(*)>5;
    if (select_sql.having_conditions != nullptr) {
      rc = FilterStmt::create(db,
          default_table,
          &table_map,
          select_sql.having_conditions,
          having_filter_stmt);
      if (rc != RC::SUCCESS) {
        LOG_WARN("cannot construct filter stmt");
        return rc;
      }
      // a. create filter stmt 中 ，having 子句中的已经内容进行 check_filed 了,并且 如果是 agg_expr，就先取出来
      auto & filter_expr = having_filter_stmt->condition();
      filter_expr->traverse(collect_aggr_exprs);//提取所有 aggexpr
      filter_expr->traverse(collect_field_exprs );//提取 select clause 中的所有 field_expr,传递给groupby stmt
      //project->traverse(collect_field_exprs, [](const Expression* expr) { return expr->type() != ExprType::AGGRFUNCTION; });
      //提取所有不在 aggexpr 中的 field_expr，用于语义检查
      filter_expr->traverse(collect_exprs_not_aggexpr,[](const Expression* expr) { return expr->type() != ExprType::AGGRFUNCTION; });
      select_sql.having_conditions = nullptr;
    }

    // 2. 语义检查 check:
    // - 聚集函数参数个数、参数为 * 的检查是在 syntax parser 完成
    // - 聚集函数中的字段 OK select clause 检查过了

    // - 没有 group by clause 时，不应该有非聚集函数中的字段
    if (!field_exprs_not_aggr.empty() && select_sql.groupby_exprs.size() == 0) {
      LOG_WARN("No Group By. But Has Fields Not In Aggr Func");
      return RC::INVALID_ARGUMENT;
    }

    // - 有 group by，要判断 select clause 和 having clause 中的 expr (除 agg_expr) 在一个 group 中只能有一个值
    // e.g. select min(c1), c2+c3*c4 from t1 group by c2+c3, c4; YES
    //      select min(c1), c2, c3+c4 from t1 group by c2+c3;    NO
    if (select_sql.groupby_exprs.size() > 0) {
      // do check field
      for (size_t i = 0; i < select_sql.groupby_exprs.size(); i++) {
        Expression* expr = select_sql.groupby_exprs[i];
        if (rc = expr->traverse_check(check_project_expr); rc != RC::SUCCESS) {
          LOG_WARN("project expr traverse check_field error!");
          return rc;
        }
      }

      // 先提取 select 后的非 aggexpr ，然后判断其是否是 groupby 中
      std::vector<Expression*> &groupby_exprs =  select_sql.groupby_exprs;
      auto check_expr_in_groupby_exprs = [&groupby_exprs](std::unique_ptr<Expression>& expr) {
        for(auto tmp : groupby_exprs) {
          if(expr->name() == tmp->name()) //通过表达式的名称进行判断
            return true;
        }
        return false;
      };

      // TODO 没有检查 having 和 order by 子句中的表达式
      for (auto& project : projects) {
        if(project->type() != ExprType::AGGRFUNCTION) {
          if(!check_expr_in_groupby_exprs(project)) {
            LOG_WARN("expr not in groupby_exprs");
            return RC::INVALID_ARGUMENT;
          }
        }
      }
    }

    // 3. create groupby stmt
    rc = GroupByStmt::create(db,
        default_table,
        &table_map,
        select_sql.groupby_exprs,
        groupby_stmt,
        std::move(aggr_exprs),
        std::move(field_exprs));
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct groupby stmt");
      return rc;
    }
    select_sql.groupby_exprs.clear();
    // 4. 在物理计划生成阶段向 groupby_operator 下挂一个 orderby_operator
  }

  OrderByStmt *orderby_stmt = nullptr; // TODO release memory when failed
  // create orderby stmt
  // 因为我们 order by 的实现要拷贝所有需要的数据 所以还是要提取 TODO 这里可能会重复 但是先不考虑
  // - 先提取 select clause 后的 field_expr(非agg_expr中的)，和 agg_expr，这里提取时已经不需要再进行 check 了，因为在 select clause
  // - order by 后的 expr 进行 check field
  if(select_sql.orderbys.size() > 0) {
    // 提取 AggrFuncExpr 以及不在 AggrFuncExpr 中的 FieldExpr
    std::vector<std::unique_ptr<Expression>> expr_for_orderby;
    // 用于从 project exprs 中提取所有 aggr func exprs. e.g. min(c1 + 1) + 1
    auto collect_aggr_exprs = [&expr_for_orderby](Expression * expr) {
      if (expr->type() == ExprType::AGGRFUNCTION) {
        expr_for_orderby.emplace_back(static_cast<AggrFuncExpr*>(static_cast<AggrFuncExpr*>(expr)->deep_copy().release()));
      }
    };
    // 用于从 project exprs 中提取所有不在 aggr func expr 中的 field expr
    auto collect_field_exprs = [&expr_for_orderby](Expression * expr) {
      if (expr->type() == ExprType::FIELD) {
        expr_for_orderby.emplace_back(static_cast<FieldExpr*>(static_cast<FieldExpr*>(expr)->deep_copy().release()));
      }
    };
    // do extract
    for (auto& project : projects) {
      project->traverse(collect_aggr_exprs);
      project->traverse(collect_field_exprs, [](const Expression* expr) { return expr->type() != ExprType::AGGRFUNCTION; });
    }
    // TODO 检查应该放到 create 里面去检查
    // do check field
    for (size_t i = 0 ; i < select_sql.orderbys.size() ; i++){
      Expression* expr = select_sql.orderbys[i].expr;
      if (rc = expr->traverse_check(check_project_expr); rc != RC::SUCCESS) {
      LOG_WARN("project expr traverse check_field error!");
      return rc;
      }
    }
    rc = OrderByStmt::create(db,
      default_table,
      &table_map,
      select_sql.orderbys,
      orderby_stmt,
      std::move(expr_for_orderby));
    if (RC::SUCCESS != rc) {
      return rc;
    }
    select_sql.orderbys.clear();
  }

  // everything alright
  // NOTE: 此时 select_sql 原有的部分信息已被移除 后续不得使用
  SelectStmt *select_stmt = new SelectStmt();
  select_stmt->join_tables_.swap(join_tables);
  select_stmt->projects_.swap(projects);
  select_stmt->filter_stmt_ = filter_stmt; // maybe nullptr
  select_stmt->groupby_stmt_ = groupby_stmt; // maybe nullptr
  select_stmt->orderby_stmt_ = orderby_stmt; // maybe nullptr
  select_stmt->having_stmt_ = having_filter_stmt; // maybe nullptr
  stmt = select_stmt;
  return RC::SUCCESS;
}
