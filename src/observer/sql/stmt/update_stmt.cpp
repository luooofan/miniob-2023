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
UpdateStmt::UpdateStmt(Table *table, const Value *values, int value_amount,FieldMeta field,FilterStmt * filter_stmt)
    : table_(table), values_(values), value_amount_(value_amount),filter_stmt_(filter_stmt)
{
  fields_.push_back(field);
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
  // TODO
    const char *table_name = update.relation_name.c_str();
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p",db, table_name);
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
  const TableMeta &table_meta = table->table_meta();
  const int sys_field_num = table_meta.sys_field_num();
   //1.检查 表t1 有没有c1 列
   //2.检查 c1 列的类型 与 1 是否匹配
  const std::vector<FieldMeta>* fieldMeta = table_meta.field_metas();
  bool valid = false;
  FieldMeta update_field;
  for ( FieldMeta field :*fieldMeta) {
    if( 0 == strcmp(field.name(),update.attribute_name.c_str()))
    {
      if(field.type() == update.value.attr_type())
      {
        if(field.type() == CHARS && field.len() < update.value.length())
        {
            return RC::INVALID_ARGUMENT;
        }
        valid = true;
        update_field = field;
        break;
      }
    }
  }
  if(!valid)
  {
    LOG_WARN("update field type mismatch. table=%s",table_name);
    return RC::INVALID_ARGUMENT;
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
  stmt = new UpdateStmt(table, &(update.value), 1,update_field,filter_stmt);
  return RC::SUCCESS;
}
