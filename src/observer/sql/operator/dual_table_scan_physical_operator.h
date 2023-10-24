/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include "sql/operator/physical_operator.h"

/**
 * @brief 空表扫描算子 只是为了给 ProjectOperator 提供一个孩子 使用内部的 EmptyTuple
 * @ingroup PhysicalOperator
 */
class DualTableScanPhysicalOperator : public PhysicalOperator
{
public:
  DualTableScanPhysicalOperator() = default;
  virtual ~DualTableScanPhysicalOperator() = default;

  PhysicalOperatorType type() const override
  {
    return PhysicalOperatorType::DUAL_TABLE_SCAN;
  }

  RC open(Trx *trx) override { return RC::SUCCESS; }

  RC next() override
  {
    if (is_first_) {
      is_first_ = false;
      return RC::SUCCESS;
    }
    return RC::RECORD_EOF;
  }

  RC close() override { return RC::SUCCESS; }

  Tuple *current_tuple() override { return &tuple_; }

private:
  bool is_first_ = true;
  EmptyTuple tuple_;
};
