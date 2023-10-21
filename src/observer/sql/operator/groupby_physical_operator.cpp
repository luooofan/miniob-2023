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
// Created by WangYunlai on 2022/6/27.
//

#include "common/log/log.h"
#include "sql/operator/groupby_physical_operator.h"
#include "storage/record/record.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/field/field.h"

GroupByPhysicalOperator::GroupByPhysicalOperator(std::vector<GroupByUnit*>& groupby_units,std::vector<AggrFuncExpr*> &agg_exprs,
std::vector<FieldExpr*> &field_exprs): groupby_units_(groupby_units)
{
  tuple_.init(groupby_units_,agg_exprs,field_exprs);
}

RC GroupByPhysicalOperator::open(Trx *trx)
{
  RC rc = RC::SUCCESS;
  if (children_.size() != 1) {
    LOG_WARN("GroupByPhysicalOperator must has one child");
    return RC::INTERNAL;
  }
  if (RC::SUCCESS != (rc = children_[0]->open(trx))) {
    rc = RC::INTERNAL;
    LOG_WARN("SortOperater child open failed!");
  }
  tuple_.set_tuple(children_[0] -> current_tuple());
  is_record_eof_ = false;
  is_first_ = true;
  is_new_group_ = true;
  return rc;
}

RC GroupByPhysicalOperator::next()
{
  if(is_record_eof_)
  {
    return RC::RECORD_EOF;
  }
  RC rc = RC::SUCCESS;
  if(is_first_)
  {
    rc = children_[0]->next();
    is_first_ = false;
    is_new_group_ = true;
    if (RC::SUCCESS != rc) 
    {
      return rc;
    }
    // set initial value of pre_values_
    for(auto unit : groupby_units_)
    {
      Value val;
      unit->expr()->get_value(*children_[0]->current_tuple(),val);
      pre_values_.emplace_back(val);
    }
    assert(pre_values_.size() == groupby_units_.size());
    LOG_INFO("GroupByOperator set first success!");
  }

  while(true)
  {
      // 0. if the last row is new group, do aggregate first
      if(is_new_group_)
      {
        tuple_.do_aggregate_first();
        is_new_group_ = false;
      }
      if(RC::SUCCESS != (rc = children_[0]->next()))
      {
        break;
      }
      // 1. adjust whether current tuple is new group or not
      for(size_t i = 0; i < groupby_units_.size(); ++i)
      {
        const GroupByUnit *unit = groupby_units_[i];
        Expression *expr = unit->expr();
        Value value;
        expr->get_value(*children_[0]->current_tuple(),value);
        if(value.compare(pre_values_[i]) != 0)
        {
            // 2. update pre_values_ and set new group
            pre_values_[i] = value;
            is_new_group_ = true;
        }
      }
    // 3. if new group, should return a row
    if(is_new_group_)
    {
      tuple_.do_aggregate_done();
      return rc;
    }
    // 4. if not new group, execute aggregate function and update result
    tuple_.do_aggregate();
  }//end while

  if (RC::RECORD_EOF == rc) 
  {
    is_record_eof_ = true;
    tuple_.do_aggregate_done();
    return RC::SUCCESS;
  }
  return rc;
}

RC GroupByPhysicalOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple *GroupByPhysicalOperator::current_tuple()
{
  return &tuple_;
}
