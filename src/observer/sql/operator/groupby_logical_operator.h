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
// Created by NiuXiangNan on 2023/4/25.
//

#pragma once

#include <vector>

#include "sql/operator/logical_operator.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/value.h"
#include "storage/field/field.h"
#include "sql/stmt/groupby_stmt.h"
/**
 * @brief 逻辑算子
 * @ingroup LogicalOperator
 */
class GroupByLogicalOperator : public LogicalOperator
{
public:
  GroupByLogicalOperator(std::vector<GroupByUnit*> &group_units,std::vector<AggrFuncExpr*>&agg_exprs,std::vector<FieldExpr*>&field_exprs);
  virtual ~GroupByLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::GROUPBY;
  }
  std::vector<GroupByUnit*>& groupby_units()
  {
    return groupby_units_;
  }
  std::vector<AggrFuncExpr*>&agg_exprs()
  {
    return agg_exprs_;
  }
  std::vector<FieldExpr*>&field_exprs()
  {
    return field_exprs_;
  }
private:
  std::vector<GroupByUnit*>groupby_units_;
  std::vector<AggrFuncExpr*>agg_exprs_;
  std::vector<FieldExpr*>field_exprs_;
};