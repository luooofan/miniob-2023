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
// Created by Wangyunlai on 2022/5/22.
//

#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/defer.h"
#include "common/lang/string.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

FilterStmt::~FilterStmt()
{
  for (FilterUnit *unit : filter_units_) {
    delete unit;
  }
  filter_units_.clear();
}

RC FilterStmt::create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const ConditionSqlNode *conditions, int condition_num, FilterStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  FilterStmt *tmp_stmt = new FilterStmt();
  for (int i = 0; i < condition_num; i++) {
    FilterUnit *filter_unit = nullptr;
    rc = create_filter_unit(db, default_table, tables, conditions[i], filter_unit);
    if (rc != RC::SUCCESS) {
      delete tmp_stmt;
      LOG_WARN("failed to create filter unit. condition index=%d", i);
      return rc;
    }
    tmp_stmt->filter_units_.push_back(filter_unit);
  }

  stmt = tmp_stmt;
  return rc;
}

RC FilterStmt::create_filter_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const ConditionSqlNode &condition, FilterUnit *&filter_unit)
{
  RC rc = RC::SUCCESS;

  CompOp comp = condition.comp;
  if (comp < EQUAL_TO || comp >= NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  filter_unit = new FilterUnit; //filterUnit 左右应该都是表达式
  DEFER([&](){
    if (RC::SUCCESS != rc && nullptr != filter_unit) {
      delete filter_unit;
      filter_unit = nullptr;
    }
  });

  const std::vector<Table *> table_arr; // 因为条件表达式里的 FieldExpr 一定是 t1.c1 所以传入个空的 table vector 就行
  rc = condition.left_expr->check_field(*tables, table_arr, db, default_table);
  if(rc != RC::SUCCESS ) {
    LOG_WARN("filter_stmt check_field lhs expression error");
    return rc;
  }
  rc = condition.right_expr->check_field(*tables, table_arr, db, default_table);
  if(rc != RC::SUCCESS) {
    LOG_WARN("filter_stmt check_field rhs expression error");
    return rc;
  }

  filter_unit->set_left(std::unique_ptr<Expression>(condition.left_expr));
  // condition.left_expr = nullptr;
  filter_unit->set_right(std::unique_ptr<Expression>(condition.right_expr));
  // condition.right_expr = nullptr;
  filter_unit->set_comp(comp);
  // 检查两个类型是否能够比较
  return rc;
}
