/* Copyright (c) 2023 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2023/08/16.
//

#include "sql/optimizer/logical_plan_generator.h"

#include "sql/operator/logical_operator.h"
#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/create_table_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/update_logical_operator.h"
#include "sql/operator/groupby_logical_operator.h"
#include "sql/operator/orderby_logical_operator.h"

#include "sql/stmt/stmt.h"
#include "sql/stmt/calc_stmt.h"
#include "sql/stmt/create_table_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/insert_stmt.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/explain_stmt.h"
#include "sql/stmt/update_stmt.h"
#include "sql/stmt/orderby_stmt.h"

using namespace std;

RC LogicalPlanGenerator::create(Stmt *stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;
  switch (stmt->type()) {
    case StmtType::CALC: {
      CalcStmt *calc_stmt = static_cast<CalcStmt *>(stmt);
      rc = create_plan(calc_stmt, logical_operator);
    } break;

    case StmtType::CREATE_TABLE: {
      CreateTableStmt *create_table_stmt = static_cast<CreateTableStmt *>(stmt);
      rc = create_plan(create_table_stmt, logical_operator);
    } break;

    case StmtType::SELECT: {
      SelectStmt *select_stmt = static_cast<SelectStmt *>(stmt);
      rc = create_plan(select_stmt, logical_operator);
    } break;

    case StmtType::INSERT: {
      InsertStmt *insert_stmt = static_cast<InsertStmt *>(stmt);
      rc = create_plan(insert_stmt, logical_operator);
    } break;

    case StmtType::DELETE: {
      DeleteStmt *delete_stmt = static_cast<DeleteStmt *>(stmt);
      rc = create_plan(delete_stmt, logical_operator);
    } break;

    case StmtType::EXPLAIN: {
      ExplainStmt *explain_stmt = static_cast<ExplainStmt *>(stmt);
      rc = create_plan(explain_stmt, logical_operator);
    } break;

    case StmtType::UPDATE: {
      UpdateStmt *update_stmt = static_cast<UpdateStmt *>(stmt);
      rc = create_plan(update_stmt, logical_operator);
    } break;
    default: {
      rc = RC::UNIMPLENMENT;
    }
  }
  return rc;
}

RC LogicalPlanGenerator::create_plan(CalcStmt *calc_stmt, std::unique_ptr<LogicalOperator> &logical_operator)
{
  logical_operator.reset(new CalcLogicalOperator(std::move(calc_stmt->expressions())));
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(CreateTableStmt *create_stmt, std::unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;
  std::unique_ptr<LogicalOperator> select_oper;
  Stmt *create_select_stmt =  create_stmt->get_create_table_select_stmt();
  if (nullptr != create_select_stmt) {
    SelectStmt *select_stmt = static_cast<SelectStmt*>(create_select_stmt);
    rc = create_plan(select_stmt, select_oper);
    if (RC::SUCCESS != rc) {
      LOG_WARN("failed to create sub select logical plan, r=%s", strrc(rc));
      return rc;
    }
  }

  logical_operator = std::unique_ptr<LogicalOperator>(new CreateTableLogicalOperator(
                                                            create_stmt->get_db(), 
                                                            create_stmt->table_name(), 
                                                            create_stmt->attr_infos()));
  if (select_oper) {
    logical_operator->add_child(std::move(select_oper));
  }
  return rc;
}

RC LogicalPlanGenerator::create_plan(
    SelectStmt *select_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;

  const std::vector<SelectStmt::JoinTables> &tables = select_stmt->join_tables();
  // const std::vector<Field> &all_fields = select_stmt->query_fields();

  auto process_one_table = [/*, &all_fields*/](unique_ptr<LogicalOperator>& prev_oper, Table* table, FilterStmt* fu) {
    std::vector<Field> fields; // TODO(wbj) 现在没用这个
    // for (const Field &field : all_fields) {
    //   if (0 == strcmp(field.table_name(), table->name())) {
    //     fields.push_back(field);
    //   }
    // }
    unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, fields, true/*readonly*/));
    unique_ptr<LogicalOperator> predicate_oper;
    if (nullptr != fu) {
      if (RC rc = LogicalPlanGenerator::create_plan(fu, predicate_oper); rc != RC::SUCCESS) {
        LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
        return rc;
      }
    }
    if (prev_oper == nullptr) {
      // ASSERT(nullptr == fu, "ERROR!");
      if (predicate_oper) {
        static_cast<TableGetLogicalOperator*>(table_get_oper.get())->set_predicates(std::move(predicate_oper->expressions()));
      }
      prev_oper = std::move(table_get_oper);
    } else {
      unique_ptr<JoinLogicalOperator> join_oper = std::make_unique<JoinLogicalOperator>();
      join_oper->add_child(std::move(prev_oper));
      join_oper->add_child(std::move(table_get_oper));
      if (predicate_oper) {
        predicate_oper->add_child(std::move(join_oper));
        prev_oper = std::move(predicate_oper);
      } else {
        prev_oper = std::move(join_oper);
      }
    }
    return RC::SUCCESS;
  };

  unique_ptr<LogicalOperator> outside_prev_oper(nullptr); // 笛卡尔积
  for (auto& jt : tables) {
    unique_ptr<LogicalOperator> prev_oper(nullptr); // INNER JOIN
    auto& join_tables = jt.join_tables();
    auto& on_conds = jt.on_conds();
    ASSERT(join_tables.size() == on_conds.size(), "ERROR!");
    for (size_t i = 0; i < join_tables.size(); ++i) {
      if (rc = process_one_table(prev_oper, join_tables[i], on_conds[i]); RC::SUCCESS != rc) {
        return rc;
      }
    }
    // now combine outside_prev_oper and prev_oper
    if (outside_prev_oper == nullptr) {
      outside_prev_oper = std::move(prev_oper);
    } else {
      unique_ptr<JoinLogicalOperator> join_oper = std::make_unique<JoinLogicalOperator>();
      join_oper->add_child(std::move(outside_prev_oper));
      join_oper->add_child(std::move(prev_oper));
      outside_prev_oper = std::move(join_oper);
    }
  }

  // set top oper
  ASSERT(outside_prev_oper, "ERROR!"); // TODO(wbj) Why doesn't work?
  unique_ptr<LogicalOperator> top_oper = std::move(outside_prev_oper); // maybe null

  if (select_stmt->filter_stmt() != nullptr) {
    unique_ptr<LogicalOperator> predicate_oper;
    rc = create_plan(select_stmt->filter_stmt(), predicate_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
      return rc;
    }
    if (predicate_oper) {
      if (top_oper) {
        predicate_oper->add_child(std::move(top_oper));
      }
      top_oper = std::move(predicate_oper);
    }
  }
  if (select_stmt->groupby_stmt()) {
    // 为 groupby_oper 加一个sort 子算子
    // 1.先构造 orderby_unit
    auto & group_fields = select_stmt->groupby_stmt()->get_groupby_fields();
    std::vector<unique_ptr<OrderByUnit>> order_units;
    for(auto &expr: group_fields)
    {
      order_units.emplace_back(std::make_unique<OrderByUnit>(expr->deep_copy().release(),true));//这里指针需要深拷贝一份给 order by
    }

  //  2 .需要将 groupy_oper 中的 field_expr ,和 groupby  后的expr  复制一份传递给 orderby 算子
    std::vector<std::unique_ptr<Expression>> field_exprs;
    auto & field = select_stmt->groupby_stmt()->get_field_exprs();
    for(auto &expr : field)
    {
      field_exprs.emplace_back(expr->deep_copy().release());
    }
    auto & tmp = select_stmt->groupby_stmt()->get_groupby_fields();
    for(auto & expr :tmp )
    {
      field_exprs.emplace_back(expr->deep_copy().release());
    }

    if (!select_stmt->groupby_stmt()->get_groupby_fields().empty()) {
      unique_ptr<LogicalOperator> orderby_oper(new OrderByLogicalOperator(std::move(order_units),std::move(field_exprs)));
      if (top_oper) {
        orderby_oper->add_child(std::move(top_oper));
      }
      top_oper = std::move(orderby_oper);
    }

    unique_ptr<LogicalOperator> groupby_oper;
    rc = create_plan(select_stmt->groupby_stmt(), groupby_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create groupby logical plan. rc=%s", strrc(rc));
      return rc;
    }
    
    groupby_oper->add_child(std::move(top_oper));
    top_oper = std::move(groupby_oper);

    // if(groupby_oper){
    //   if (top_oper) {
    //     groupby_oper->add_child(std::move(top_oper));
    //   }
    //   top_oper = std::move(groupby_oper);
    // }
  }

  if (select_stmt->having_stmt() != nullptr) {
    unique_ptr<LogicalOperator> predicate_oper;
    rc = create_plan(select_stmt->having_stmt(), predicate_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create having predicate logical plan. rc=%s", strrc(rc));
      return rc;
    }
    if (predicate_oper) {
      if (top_oper) {
        predicate_oper->add_child(std::move(top_oper));
      }
      top_oper = std::move(predicate_oper);
    }
  }

  if (select_stmt->orderby_stmt()) {
    unique_ptr<LogicalOperator> orderby_oper;
    rc = create_plan(select_stmt->orderby_stmt(), orderby_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create orderby logical plan. rc=%s", strrc(rc));
      return rc;
    }
    if(orderby_oper){
      if (top_oper) {
        orderby_oper->add_child(std::move(top_oper));
      }
      top_oper = std::move(orderby_oper);
    }
  }
  {
    unique_ptr<LogicalOperator> project_oper(new ProjectLogicalOperator(std::move(select_stmt->projects())));
    ASSERT(project_oper,"ERROR!");
    if (top_oper) {
      project_oper->add_child(std::move(top_oper));
    }
    top_oper = std::move(project_oper);
  }

  logical_operator.swap(top_oper);
  return RC::SUCCESS;
}

// you have to std::move(cmp_exprs)
unique_ptr<PredicateLogicalOperator> cmp_exprs2predicate_logic_oper(std::vector<unique_ptr<Expression>> cmp_exprs) {
  if (!cmp_exprs.empty()) {
    unique_ptr<ConjunctionExpr> conjunction_expr(new ConjunctionExpr(ConjunctionExpr::Type::AND, std::move(cmp_exprs)));
    return std::make_unique<PredicateLogicalOperator>(std::move(conjunction_expr));
  }
  return {};
}

RC LogicalPlanGenerator::create_plan(
    FilterStmt *filter_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  if (filter_stmt == nullptr || filter_stmt->condition() == nullptr) {
    return {};
  }
  std::vector<unique_ptr<Expression>> cmp_exprs;
  // 给子查询生成 logical oper
  auto process_sub_query = [](Expression* expr) {
    if (expr->type() == ExprType::SUBQUERY) {
      SubQueryExpr* sub_query_expr = static_cast<SubQueryExpr*>(expr);
      return sub_query_expr->generate_logical_oper();
    }
    return RC::SUCCESS;
  };
  if (RC rc = filter_stmt->condition()->traverse_check(process_sub_query); OB_FAIL(rc)) {
    return rc;
  }
  cmp_exprs.emplace_back(std::move(filter_stmt->condition()));
  logical_operator = cmp_exprs2predicate_logic_oper(std::move(cmp_exprs));
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(GroupByStmt *group_by_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  if (group_by_stmt == nullptr) {
    logical_operator = nullptr;
    return RC::SUCCESS;
  }

  unique_ptr<LogicalOperator> groupby_oper(
    new GroupByLogicalOperator(std::move(group_by_stmt->get_groupby_fields()),
                               std::move(group_by_stmt->get_agg_exprs()),
                               std::move(group_by_stmt->get_field_exprs())));
  logical_operator = std::move(groupby_oper);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(OrderByStmt *order_by_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  if (order_by_stmt == nullptr) {
    logical_operator = nullptr;
    return RC::SUCCESS;
  }

  unique_ptr<LogicalOperator> orderby_oper(
    new OrderByLogicalOperator(std::move(order_by_stmt->get_orderby_units()),
                               std::move(order_by_stmt->get_exprs())));
  logical_operator = std::move(orderby_oper);
  return RC::SUCCESS;
}

/*
RC LogicalPlanGenerator::create_plan(
    FilterUnit *filter_unit, unique_ptr<LogicalOperator> &logical_operator)
{
  std::vector<unique_ptr<Expression>> cmp_exprs;
  ComparisonExpr *cmp_expr = new ComparisonExpr(filter_unit->comp(),  std::move(filter_unit->left()), std::move(filter_unit->right()));
  cmp_exprs.emplace_back(cmp_expr);

  logical_operator = cmp_exprs2predicate_logic_oper(std::move(cmp_exprs));
  return RC::SUCCESS;
}
*/

RC LogicalPlanGenerator::create_plan(
    InsertStmt *insert_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table *table = insert_stmt->table();
  vector<vector<Value>> values;
  for (int i = 0; i < insert_stmt->values().size(); ++i) {
    values.emplace_back(insert_stmt->values()[i]);
  }

  InsertLogicalOperator *insert_operator = new InsertLogicalOperator(table, std::move(values));
  logical_operator.reset(insert_operator);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    DeleteStmt *delete_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table *table = delete_stmt->table();
  FilterStmt *filter_stmt = delete_stmt->filter_stmt();
  std::vector<Field> fields;
  for (int i = table->table_meta().sys_field_num(); i < table->table_meta().field_num(); i++) {
    const FieldMeta *field_meta = table->table_meta().field(i);
    fields.push_back(Field(table, field_meta));
  }
  unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, fields, false/*readonly*/));

  unique_ptr<LogicalOperator> predicate_oper;
  RC rc = create_plan(filter_stmt, predicate_oper);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  unique_ptr<LogicalOperator> delete_oper(new DeleteLogicalOperator(table));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_get_oper));
    delete_oper->add_child(std::move(predicate_oper));
  } else {
    delete_oper->add_child(std::move(table_get_oper));
  }

  logical_operator = std::move(delete_oper);
  return rc;
}

RC LogicalPlanGenerator::create_plan(
    ExplainStmt *explain_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Stmt *child_stmt = explain_stmt->child();
  unique_ptr<LogicalOperator> child_oper;
  RC rc = create(child_stmt, child_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create explain's child operator. rc=%s", strrc(rc));
    return rc;
  }

  logical_operator = unique_ptr<LogicalOperator>(new ExplainLogicalOperator);
  logical_operator->add_child(std::move(child_oper));
  return rc;
}

RC LogicalPlanGenerator::create_plan(
    UpdateStmt *update_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table *table = update_stmt->table();
  FilterStmt *filter_stmt = update_stmt->filter_stmt();
  std::vector<Field> fields;
  for (int i = table->table_meta().sys_field_num(); i < table->table_meta().field_num(); i++) {
    const FieldMeta *field_meta = table->table_meta().field(i);
    fields.push_back(Field(table, field_meta));
  }
  unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, fields, false/*readonly*/));

  unique_ptr<LogicalOperator> predicate_oper;
  RC rc = RC::SUCCESS;
  if(filter_stmt != nullptr)
  {
    RC rc = create_plan(filter_stmt, predicate_oper);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  auto process_sub_query = [](std::unique_ptr<Expression>& expr) {
    if (expr->type() == ExprType::SUBQUERY) {
      SubQueryExpr* sub_query_expr = static_cast<SubQueryExpr*>(expr.get());
      return sub_query_expr->generate_logical_oper();
    }
    return RC::SUCCESS;
  };
  for (auto& value : update_stmt->values()) {
    rc = process_sub_query(value);
    if (RC::SUCCESS != rc) {
      return rc;
    }
  }
  unique_ptr<LogicalOperator> update_oper(new UpdateLogicalOperator(table, std::move(update_stmt->values()), update_stmt->update_fields()));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_get_oper));
    update_oper->add_child(std::move(predicate_oper));
  } else {
    update_oper->add_child(std::move(table_get_oper));
  }

  logical_operator = std::move(update_oper);
  return rc;
}