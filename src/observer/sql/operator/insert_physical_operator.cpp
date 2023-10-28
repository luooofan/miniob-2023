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
#include "storage/field/field.h"
#include "storage/table/view.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"

using namespace std;

InsertPhysicalOperator::InsertPhysicalOperator(BaseTable *base_table, std::vector<vector<Value>> &&values)
    : base_table_(base_table), values_(std::move(values))
{}

RC InsertPhysicalOperator::open(Trx *trx)
{
  RC rc = RC::SUCCESS;
  if (base_table_->is_table()) {
    rc = insert_into_table(trx);
    if (RC::SUCCESS != rc) {
      LOG_WARN("failed to insert into table %s, rc=%s", base_table_->name(), strrc(rc));
    }
  } else {
    rc = insert_into_view(trx);
    if (RC::SUCCESS != rc) {
      LOG_WARN("failed to insert into view %s, rc=%s", base_table_->name(), strrc(rc));
    }
  } 
  return rc;
}

RC InsertPhysicalOperator::insert_into_table(Trx *trx)
{
  RC rc = RC::SUCCESS;
  std::vector<Record> records;
  records.resize(values_.size());
  Table *table_ = static_cast<Table*>(base_table_);
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

RC InsertPhysicalOperator::insert_into_view(Trx *trx)
{
  RC rc = RC::SUCCESS;
  View *view = static_cast<View*>(base_table_);
  if (!view->allow_write()) {
    LOG_ERROR("view %s is not allow to insert", view->name());
    return RC::SCHEMA_FIELD_MISSING;
  }

  /*  
   *  首先将map_field按照表分类，记录下每张表中有哪些view_col、
   *  | table_1: view_col_1, view_col_3 | table_2: view_col_2 |
   *  
   *  记录view_col在原始表中的位置
   *  | table_1: 3, 1 | table_2: 2 |
   */  
  const std::vector<Field> map_fields = view->get_map_fields();
  std::unordered_map<const BaseTable*, std::vector<int>> view_col_idx;
  for (int i = 0; i < map_fields.size(); i++) {
    const Field &field = map_fields[i];
    auto iter = view_col_idx.find(field.table());
    if (view_col_idx.end() == iter) {
      view_col_idx.emplace(field.table(), std::vector<int>{i});
    } else {
      iter->second.emplace_back(i);
    }
  }

  // 查找view中的列在原始表中的位置
  std::unordered_map<const BaseTable*, std::vector<int>> field_idx_in_tables;
  for (auto iter : view_col_idx) {
    std::vector<int> field_idx_in_one_table;
    const BaseTable *table = iter.first;
    const int sys_field_num = table->table_meta().sys_field_num();
    std::vector<int> cols_in_table = iter.second;
    for (auto col_idx : cols_in_table) {
      int field_idx = table->table_meta().find_field_idx_by_name(map_fields[col_idx].field_name());
      field_idx_in_one_table.emplace_back(field_idx - sys_field_num);
    }
    field_idx_in_tables.emplace(table, field_idx_in_one_table);
  }

  // TODO: 检查缺省列是否允许为NULL
  // 按顺序对每张表 补齐Value，构造Record
  std::vector<std::vector<Record>> multi_table_records;
  for (auto col_of_table : view_col_idx) {
    // 对一张原始表的插入
    const BaseTable *base_table = col_of_table.first;
    Table *table = static_cast<Table*>(const_cast<BaseTable*>(base_table));

    const std::vector<int> &field_idx_in_one_table = field_idx_in_tables[table];

    for (std::vector<Value> row_value : values_) {
      // 补齐一行数据
      std::vector<Value> fixed_row_value(table->table_meta().field_num() - table->table_meta().sys_field_num(), {NULLS, nullptr, 0});
      for (size_t i = 0; i < field_idx_in_one_table.size(); i++) {
        fixed_row_value[field_idx_in_one_table[i]] = row_value[col_of_table.second[i]];
      }

      // 插入一行数据
      Record rcd;
      rc = table->make_record(static_cast<int>(fixed_row_value.size()), fixed_row_value.data(), rcd);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to make record. rc=%s", strrc(rc));
        return rc;
      }
      rc = trx->insert_record(table, rcd);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to insert record by transaction. rc=%s", strrc(rc));
        // 插入失败，需要回滚之前插入成功的记录
        // RC rc2 = RC::SUCCESS;
        // for (int j = i - 1; j >= 0; j--) {
        //   Record &done_rcd = records[j];
        //   rc2 = trx->delete_record(table_, done_rcd);
        //   if (RC::SUCCESS != rc2) {
        //     LOG_WARN("failed to rollback record after insert failed. rc=%s", strrc(rc2));
        //     break;
        //   }
        // }
        break;  // 插入失败，回滚后应该停止继续插入
      }
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
