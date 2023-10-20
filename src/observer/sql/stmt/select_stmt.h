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
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include <vector>
#include <memory>

#include "common/rc.h"
#include "sql/stmt/stmt.h"
#include "storage/field/field.h"
#include "sql/executor/sql_result.h"
#include "event/sql_event.h"

class SQLStageEvent;
class FieldMeta;
class FilterStmt;
class Db;
class Table;

/**
 * @brief 表示select语句
 * @ingroup Statement
 */
class SelectStmt : public Stmt 
{
public:
  class JoinTables {
  public:
    JoinTables() = default;
    ~JoinTables() = default;
    JoinTables(JoinTables&& other) {
      join_tables_.swap(other.join_tables_);
      on_conds_.swap(other.on_conds_);
    }
    void push_join_table(Table* table, FilterStmt* fu) {
      join_tables_.emplace_back(table);
      on_conds_.emplace_back(fu);
    }
    const std::vector<Table*>& join_tables() const {
      return join_tables_;
    }
    const std::vector<FilterStmt*>& on_conds() const {
      return on_conds_;
    }
  private:
    std::vector<Table*> join_tables_;
    std::vector<FilterStmt*> on_conds_;
  };
public:
  SelectStmt() = default;
  ~SelectStmt() override;

  StmtType type() const override
  {
    return StmtType::SELECT;
  }

public:
  // select_sql.project exprs would be clear
  static RC create(Db *db, SelectSqlNode &select_sql, Stmt *&stmt);

public:
  const std::vector<JoinTables> &join_tables() const
  {
    return join_tables_;
  }
  FilterStmt *filter_stmt() const
  {
    return filter_stmt_;
  }
  std::vector<std::unique_ptr<Expression>> &projects()
  {
    return projects_;
  }
private:
  std::vector<std::unique_ptr<Expression>> projects_;
  std::vector<JoinTables> join_tables_;
  FilterStmt *filter_stmt_ = nullptr;
};
