#pragma once

#include <vector>
#include <unordered_map>
#include "common/rc.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/stmt.h"
#include "sql/expr/expression.h"

class Db;
class Table;
class FieldMeta;

class GroupByStmt : Stmt{
public:
  GroupByStmt() = default;
  virtual ~GroupByStmt() = default;

  StmtType type() const override
  {
    return StmtType::GROUPBY;
  }
public:
  std::vector<std::unique_ptr<AggrFuncExpr>> &get_agg_exprs()
  {
    return agg_exprs_;
  }
  std::vector<std::unique_ptr<FieldExpr>> &get_field_exprs()
  {
    return field_exprs_;
  }
  std::vector<std::unique_ptr<Expression>>& get_groupby_fields()
  {
    return groupby_fields_;
  }
  void set_agg_exprs(std::vector<std::unique_ptr<AggrFuncExpr>> &&agg_exprs)
  {
    agg_exprs_ = std::move(agg_exprs);
  }
  void set_field_exprs(std::vector<std::unique_ptr<FieldExpr>> &&field_exprs)
  {
    field_exprs_ = std::move(field_exprs);
  }
  void set_groupby_fields(std::vector<std::unique_ptr<Expression>>&& groupby_fields) {
    groupby_fields_ = std::move(groupby_fields);
  }
public:
  static RC create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      const std::vector<Expression*>& groupby_expr, GroupByStmt *&stmt,
      std::vector<std::unique_ptr<AggrFuncExpr>>&& agg_exprs,
      std::vector<std::unique_ptr<FieldExpr>>&& field_exprs);

private:
  std::vector<std::unique_ptr<Expression>> groupby_fields_; // group by clause
  std::vector<std::unique_ptr<AggrFuncExpr>> agg_exprs_; // 聚集函数表达式
  std::vector<std::unique_ptr<FieldExpr>> field_exprs_; // 非聚集函数中的字段表达式,需要传递给下层的 order by 算子

  // select min(c1) ,c2+c3 from t1 group by c2+c3,c3+c4;
};
