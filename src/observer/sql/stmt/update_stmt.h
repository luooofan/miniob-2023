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

#pragma once

#include "common/rc.h"
#include "sql/stmt/stmt.h"
#include "storage/field/field.h"
#include "sql/stmt/filter_stmt.h"
class Table;

/**
 * @brief 更新语句
 * @ingroup Statement
 */
class UpdateStmt : public Stmt 
{
public:
  UpdateStmt() = default;
  UpdateStmt(Table *table, const Value *values, int value_amount,FieldMeta field,FilterStmt *filter_stmt);
  ~UpdateStmt() override;
public:
  static RC create(Db *db, const UpdateSqlNode &update_sql, Stmt *&stmt);

public:
  Table *table() const
  {
    return table_;
  }
  const Value *values() const
  {
    return values_;
  }
  int value_amount() const
  {
    return value_amount_;
  }
  StmtType type() const override
  {
    return StmtType::UPDATE;
  }
  std::vector<FieldMeta>* update_fields()
  {
    return &fields_;
  }
  FilterStmt * filter_stmt()
  {
    return filter_stmt_;
  }
private:
  Table *table_ = nullptr;
  const Value *values_ = nullptr;
  int value_amount_ = 0;
  std::vector<FieldMeta> fields_;//将被更新的列
  FilterStmt *filter_stmt_ = nullptr;
};
