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
#include "sql/operator/delete_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "storage/table/view.h"
#include "storage/trx/trx.h"
#include "sql/stmt/delete_stmt.h"

RC DeletePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  std::unique_ptr<PhysicalOperator> &child = children_[0];
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  trx_ = trx;

  return RC::SUCCESS;
}

RC DeletePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    return RC::RECORD_EOF;
  }

  if (table_->is_table()) {
    rc = delete_from_table();
  } else {
    rc = delete_from_view();
  }

  return rc;
}

RC DeletePhysicalOperator::delete_from_table()
{
  RC rc = RC::SUCCESS;

  Table *table = static_cast<Table*>(table_);
  PhysicalOperator *child = children_[0].get();
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record &record = row_tuple->record();
    rc = trx_->delete_record(table, record);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to delete record: %s", strrc(rc));
      return rc;
    }
  }

  return RC::RECORD_EOF;
}

RC DeletePhysicalOperator::delete_from_view()
{
  RC rc = RC::SUCCESS;

  PhysicalOperator *child = children_[0].get();
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    std::unordered_map<const BaseTable*, RID> table_rid = row_tuple->get_table_rid_map();
    for (auto iter : table_rid) {
      Record record;
      if (!iter.first->is_table()) {
        LOG_ERROR("unexpect map_relation from view to view");
        return RC::INTERNAL;
      }

      Table *table = const_cast<Table*>(static_cast<const Table*>(iter.first));
      rc = table->get_record(iter.second, record);
      if (RC::SUCCESS != rc) {
        LOG_WARN("failed to get record from table:%s, rid:%ld-%ld", table->name(), iter.second.page_num, iter.second.slot_num);
        return rc;
      }

      rc = trx_->delete_record(table, record);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to delete record: %s", strrc(rc));
        return rc;
      }
    } // end delete one record from tables
  }
  return rc;
}

RC DeletePhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}
