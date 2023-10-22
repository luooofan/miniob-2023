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
// Created by WangYunlai on 2021/6/9.
//

#include "sql/operator/insert_physical_operator.h"
#include "sql/stmt/insert_stmt.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"

using namespace std;

InsertPhysicalOperator::InsertPhysicalOperator(Table *table, std::vector<vector<Value>> &&values)
    : table_(table), values_(std::move(values))
{}

RC InsertPhysicalOperator::open(Trx *trx)
{
  std::vector<Record> records;
  records.resize(values_.size());
  RC rc = RC::SUCCESS;
  for (int i = 0; i < values_.size(); ++i) {
    auto& rcd = records[i];
    rc = table_->make_record(static_cast<int>(values_[i].size()), values_[i].data(), rcd);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to make record. rc=%s", strrc(rc));
      return rc;
    }
    rc = trx->insert_record(table_, rcd);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to insert record by transaction. rc=%s", strrc(rc));
      // 插入失败，需要回滚之前插入成功的记录
      RC rc2 = RC::SUCCESS;
      for (int j = i - 1; j >= 0; j--) {
        Record &done_rcd = records[j];
        rc2 = trx->delete_record(table_, done_rcd);
        if (RC::SUCCESS != rc2) {
          LOG_WARN("failed to rollback record after insert failed. rc=%s", strrc(rc2));
          break;
        }
      }
      break;  // 插入失败，回滚后应该停止继续插入
    }
  }
  return rc;
}

RC InsertPhysicalOperator::next()
{
  return RC::RECORD_EOF;
}

RC InsertPhysicalOperator::close()
{
  return RC::SUCCESS;
}
