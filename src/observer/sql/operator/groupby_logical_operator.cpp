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

#include "sql/operator/groupby_logical_operator.h"
GroupByLogicalOperator::GroupByLogicalOperator(std::vector<GroupByUnit*> &groupby_units,std::vector<AggrFuncExpr*>& agg_exprs)
    : groupby_units_(groupby_units),agg_exprs_(agg_exprs)
{
}
