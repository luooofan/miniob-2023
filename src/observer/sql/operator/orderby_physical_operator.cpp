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
#include "sql/operator/orderby_physical_operator.h"
#include "storage/record/record.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/field/field.h"
#include <chrono>

OrderByPhysicalOperator::OrderByPhysicalOperator(std::vector<std::unique_ptr<OrderByUnit>>&& orderby_units,
  std::vector<std::unique_ptr<Expression>> &&exprs)
    : orderby_units_(std::move(orderby_units))
{
    tuple_.init(std::move(exprs));
}

RC OrderByPhysicalOperator::open(Trx *trx)
{
  RC rc = RC::SUCCESS;
  if (children_.size() != 1) {
    LOG_WARN("OrderByPhysicalOperator must has one child");
    return RC::INTERNAL;
  }
  if (RC::SUCCESS != (rc = children_[0]->open(trx))) {
    rc = RC::INTERNAL;
    LOG_WARN("GroupByOperater child open failed!");
  }
  rc = fetch_and_sort_tables();
  return rc;
}

RC OrderByPhysicalOperator::fetch_and_sort_tables()
{
  LOG_WARN("niuxn:begin sort");
  RC rc = RC::SUCCESS;

  int index = 0;
  typedef std::pair<std::vector<Value>, int> CmpPair;
  std::vector<CmpPair> pair_sort_table;//要排序的内容
  std::vector<Value> pair_cell;//参与排序的列

  std::vector<Value> row_values(tuple_.exprs().size());//缓存每一行
  int row_values_index = 0;
  //int i = 0;
  while (RC::SUCCESS == (rc = children_[0]->next())) {
    // if(i++ % 2500 == 0)
    // {
    //   LOG_WARN("niuxn:is sorting, %d",i);
    // }
    row_values_index = 0;//每一行都从 0 开始填
    // construct pair sort table
    // 1 cons vector<cell>
    pair_cell.clear();
    for (auto &unit : orderby_units_) {
      auto &expr = unit->expr();
      Value cell;
      expr->get_value(*children_[0]->current_tuple(), cell);//取出每行中要参与排序的cell
      pair_cell.emplace_back(cell);
    }
    // 2 cons pair
    // 3 cons pair vector
    pair_sort_table.emplace_back(std::make_pair(pair_cell, index++));//将每行数据放入排序的内存中
    // store child records

    //存储select 后的 fieldexpr 的值 和aggexpr 的值
    Value expr_cell;
    for(auto &expr : tuple_.exprs()){
      if(expr->get_value(*children_[0]->current_tuple(),expr_cell) != RC::SUCCESS)
      {
        LOG_WARN("error in sort");
        return RC::INTERNAL;
      }
      row_values[row_values_index++] = expr_cell;
    }
    values_.emplace_back(row_values);//values 中缓存每一行
  }
  if (RC::RECORD_EOF != rc) {
    LOG_ERROR("Fetch Table Error In SortOperator. RC: %d", rc);
    return rc;
  }
  rc = RC::SUCCESS;
  LOG_INFO("Fetch Table Success In SortOperator");

  bool order[orderby_units_.size()];  // specify 1 asc or 2 desc


  for(size_t i = 0 ; i < orderby_units_.size() ; ++i){
    order[i] = orderby_units_[i]->sort_type(); // true is asc
  }
  // consider null
  auto cmp = [&order](const CmpPair &a, const CmpPair &b) {
    auto &cells_a = a.first;
    auto &cells_b = b.first;
    assert(cells_a.size() == cells_b.size());
    for (size_t i = 0; i < cells_a.size(); ++i) {
      auto &cell_a = cells_a[i];
      auto &cell_b = cells_b[i];
      if (cell_a.is_null() && cell_b.is_null()) {
        continue;
      }
      if (cell_a.is_null()) {
        return order[i] ? true : false;
      }
      if (cell_b.is_null()) {
        return order[i] ? false : true;
      }
      if (cell_a != cell_b) {
        return order[i] ? cell_a < cell_b : cell_a > cell_b;
      }
    }
    return false;  // completely same
  };
  std::sort(pair_sort_table.begin(), pair_sort_table.end(), cmp);
  LOG_INFO("niuxn:Sort Table Success In SortOperator");

  // fill ordered_idx_
  for (size_t i = 0; i < pair_sort_table.size(); ++i) {
    ordered_idx_.emplace_back(pair_sort_table[i].second);//将原来的每行的标记写入order_index数组中
  }
  it_ = ordered_idx_.begin();

  return rc;
}


RC OrderByPhysicalOperator::next()
{

  RC rc = RC::SUCCESS;
  if (ordered_idx_.end() != it_) {
    // NOTE: PAY ATTENTION HERE

    tuple_.set_cells(&values_[*it_]);
    it_++;
    //children_[0]->current_tuple()->set_record(st_[*it_]);
    return RC::SUCCESS;
  }

  return RC::RECORD_EOF;
}

RC OrderByPhysicalOperator::close()
{
  return children_[0]->close();
}

Tuple *OrderByPhysicalOperator::current_tuple()
{
  return &tuple_;
}
