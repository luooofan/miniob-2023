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
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
UpdateStmt::UpdateStmt(Table *table, std::vector<FieldMeta> fields, std::vector<Value*> values, FilterStmt *filter_stmt)
  : table_(table), fields_(std::move(fields)), values_(std::move(values)), filter_stmt_(filter_stmt)
{
}

UpdateStmt::~UpdateStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

RC UpdateStmt::create(Db *db, const UpdateSqlNode &update, Stmt *&stmt)
{
  const char *table_name = update.relation_name.c_str();
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p",db, table_name);
    return RC::INVALID_ARGUMENT;
  }
  if (update.attribute_names.size() != update.values.size()) {
    LOG_WARN("invalid argument, column size not match value size");
    return RC::INVALID_ARGUMENT;
  }
  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  // check fields type
  // update t1 set c1 = 1;
  //1.检查 表t1 有没有c1 列
  //2.检查 c1 列的类型 与 1 是否匹配
  std::vector<Value*> values;
  std::vector<FieldMeta> fields;
  const TableMeta &table_meta = table->table_meta();
  for (size_t i = 0; i < update.attribute_names.size(); i++) {
    const FieldMeta* update_field = table_meta.field(update.attribute_names[i].c_str());
    bool valid = false;
    if (nullptr != update_field) {
      if (update_field->type() == update.values[i].attr_type() || (update.values[i].is_null() && update_field->nullable())) {
        if (update_field->type() == CHARS && update_field->len() < update.values[i].length()) {
          LOG_WARN("update chars with longer length");
        } else {
          valid = true;
        }
      }
    }
    if (!valid) {
      LOG_WARN("update field type mismatch. table=%s", table_name);
      return RC::INVALID_ARGUMENT;
    }
    fields.emplace_back(*update_field);
    values.emplace_back(const_cast<Value*>(&update.values[i]));
  }

  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(
      db, table, &table_map, update.conditions.data(), static_cast<int>(update.conditions.size()), filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    return rc;
  }
  // everything alright
  stmt = new UpdateStmt(table, std::move(fields), std::move(values), filter_stmt);
  return RC::SUCCESS;
}
