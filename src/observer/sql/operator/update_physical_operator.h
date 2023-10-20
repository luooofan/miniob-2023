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
// Created by NIUXN on 2022/6/9.
//

#pragma once

#include<vector>

#include "sql/operator/physical_operator.h"
#include "sql/parser/parse.h"

class Trx;
class UpdateStmt;

/**
 * @brief 物理算子，更新
 * @ingroup PhysicalOperator
 */
class UpdatePhysicalOperator : public PhysicalOperator
{
public:
  UpdatePhysicalOperator(Table *table, std::vector<Value*> &values, std::vector<FieldMeta> &fields) 
    : table_(table), values_(values)
  {
    for (FieldMeta &field : fields) {
      fields_.emplace_back(field.name());
    }
  }

  virtual ~UpdatePhysicalOperator() = default;

  PhysicalOperatorType type() const override
  {
    return PhysicalOperatorType::UPDATE;
  }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  // 提取待更新字段的旧值、rid，顺便检查新旧值是否重复
  RC extract_old_value(Record &record);

  Tuple *current_tuple() override
  {
    return nullptr;
  }

private:
  Table *table_ = nullptr;
  Trx *trx_ = nullptr;
  std::vector<Value*> values_;
  std::vector<std::string> fields_;

  // 存储已经更新过的行数据，用于回滚
  std::vector<RID> old_records_;
  std::vector<std::vector<Value>> old_values_;
};
