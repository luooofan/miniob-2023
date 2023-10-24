#include "sql/operator/logical_operator.h"

class Db;

/**
 * @brief CreateTable逻辑算子
 * @ingroup LogicalOperator
 */
class CreateTableLogicalOperator : public LogicalOperator 
{
public:
  CreateTableLogicalOperator(Db *db, const std::string &table_name, const std::vector<AttrInfoSqlNode> &attr_infos)
    : db_(db),table_name_(table_name), attr_infos_(attr_infos)
  {}
  virtual ~CreateTableLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::CREATE_TABLE;
  }

  std::string &table_name() 
  { 
    return table_name_; 
  }
  std::vector<AttrInfoSqlNode> &attr_infos()
  {
    return attr_infos_;
  }
  Db *get_db()
  {
    return db_;
  }

private:
  Db *db_ = nullptr;
  std::string table_name_;
  std::vector<AttrInfoSqlNode> attr_infos_;
};