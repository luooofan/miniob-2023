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

class GroupByUnit {
public:
  GroupByUnit() = default;
  GroupByUnit(Expression* expr) : expr_(expr) {}
  ~GroupByUnit() = default;

  void set_expr(Expression *expr)
  {
    expr_ = expr;
  }
  Expression *expr() const
  {
    return expr_;
  }

private:
  Expression *expr_ = nullptr; // 这里是不是只能是字段表达式 而且是独占的 看看能不能改成 unique_ptr
};

class GroupByStmt : Stmt{
public:
  GroupByStmt() = default;
  virtual ~GroupByStmt();

  StmtType type() const override
  {
    return StmtType::GROUPBY;
  }
public:
  std::vector<GroupByUnit *> &groupby_units() 
  {
    return groupby_units_;
  }
  std::vector<AggrFuncExpr *> &agg_exprs() 
  {
    return agg_exprs_;
  }
  void add_groupby_unit(GroupByUnit *unit)
  {
    groupby_units_.emplace_back(unit);
  }
  void add_agg_expr(AggrFuncExpr *expr)
  {
    agg_exprs_.emplace_back(expr);
  }
  void set_agg_exprs(std::vector<AggrFuncExpr*>&agg_exprs)
  {
    agg_exprs_ = agg_exprs;
  }
  void add_field_expr(FieldExpr *expr)
  {
    field_exprs_.emplace_back(expr);
  }
  void set_field_exprs(std::vector<FieldExpr*>&field_exprs)
  {
    field_exprs_ = field_exprs;
  }
  std::vector<FieldExpr*>& get_field_exprs()
  {
    return field_exprs_;
  }
public:
  static RC create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      const GroupBySqlNode *groupby_node, GroupByStmt *&stmt, std::vector<AggrFuncExpr*>&agg_exprs, std::vector<FieldExpr*>& field_exprs);

  static RC create_groupby_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      Expression * expr, GroupByUnit *&groupby_unit);

private:
  std::vector<GroupByUnit *> groupby_units_; // group by clause 这里应该有管理权 考虑改成 unique_ptr
  std::vector<AggrFuncExpr *> agg_exprs_; // 聚集函数表达式 这里没有管理权 管理权在 projects 中
  std::vector<FieldExpr *> field_exprs_; // 非聚集函数中的字段表达式 这里没有管理权 管理权在 projects 中
};
