#pragma once

#include <string>
#include <vector>

#include "sql/stmt/stmt.h"
#include "sql/parser/parse_defs.h"

class Db;

/**
 * @brief 描述表的语句
 * @ingroup Statement
 * @details 虽然解析成了stmt，但是与原始的SQL解析后的数据也差不多
 */
class ShowIndexStmt : public Stmt
{
public:
  ShowIndexStmt(const std::string &table_name) 
        : table_name_(table_name)
  {};

  virtual ~ShowIndexStmt() = default;

  StmtType type() const override 
  { 
    return StmtType::SHOW_INDEX; 
  }

  const std::string &table_name() const 
  { 
    return table_name_; 
  }

  static RC create(Db *db, const ShowIndexSqlNode &show_index_sql,  Stmt *&stmt);

private:
  std::string table_name_;
};