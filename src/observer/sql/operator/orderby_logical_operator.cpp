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
// Created by niuxn on 2023/4/25.
//

#include "sql/operator/orderby_logical_operator.h"
OrderByLogicalOperator::OrderByLogicalOperator(std::vector<std::unique_ptr<OrderByUnit >> &&orderby_units,
    std::vector<std::unique_ptr<Expression>> &&exprs)
    : orderby_units_(std::move(orderby_units)),
      exprs_(std::move(exprs))
{
}
