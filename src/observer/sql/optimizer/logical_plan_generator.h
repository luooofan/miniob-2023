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

#pragma once

#include <memory>

#include "common/rc.h"

class Stmt;
class CalcStmt;
class CreateTableStmt;
class SelectStmt;
class FilterUnit;
class FilterStmt;
class InsertStmt;
class DeleteStmt;
class ExplainStmt;
class UpdateStmt;
class GroupByStmt;
class OrderByStmt;
class LogicalOperator;

class LogicalPlanGenerator
{
public:
  LogicalPlanGenerator() = default;
  virtual ~LogicalPlanGenerator() = default;

  static RC create(Stmt *stmt, std::unique_ptr<LogicalOperator> &logical_operator);

private:
  static RC create_plan(CalcStmt *calc_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
  static RC create_plan(CreateTableStmt *create_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
  static RC create_plan(SelectStmt *select_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
  static RC create_plan(FilterStmt *filter_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
  // static // RC create_plan(FilterUnit *filter_unit, std::unique_ptr<LogicalOperator> &logical_operator);
  static RC create_plan(InsertStmt *insert_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
  static RC create_plan(DeleteStmt *delete_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
  static RC create_plan(ExplainStmt *explain_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
  static RC create_plan(UpdateStmt *update_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
  static RC create_plan(GroupByStmt *groupby_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
  static RC create_plan(OrderByStmt *orderby_stmt, std::unique_ptr<LogicalOperator> &logical_operator);
};