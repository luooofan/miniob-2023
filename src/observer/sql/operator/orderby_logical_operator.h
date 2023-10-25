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
#include "sql/stmt/orderby_stmt.h"
/**
 * @brief 逻辑算子
 * @ingroup LogicalOperator
 */
class OrderByLogicalOperator : public LogicalOperator
{
public:
  OrderByLogicalOperator(std::vector<std::unique_ptr<OrderByUnit >> &&orderby_units,
                         std::vector<std::unique_ptr<Expression>> &&exprs);
  virtual ~OrderByLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::ORDERBY;
  }
  std::vector<std::unique_ptr<OrderByUnit >> &orderby_units()
  {
    return orderby_units_;
  }
  std::vector<std::unique_ptr<Expression>> &exprs()
  {
    return exprs_;
  }
private:
  std::vector<std::unique_ptr<OrderByUnit >> orderby_units_; //排序列
  ///在 create order by stmt 之前提取 select clause 后的 field_expr (非a gg_expr 中的)和 agg_expr
  std::vector<std::unique_ptr<Expression>> exprs_;
};