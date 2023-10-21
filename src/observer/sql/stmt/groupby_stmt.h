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
  ~GroupByUnit()
  {
    // if (expr_) {
    //   delete expr_;
    //   expr_ = nullptr;
    // }
  }

  void set_expr(Expression *expr)
  {
    expr_ = expr;
  }
  Expression *expr() const
  {
    return expr_;
  }

private:
  Expression *expr_ = nullptr;
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

public:
  static RC create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      const GroupBySqlNode *groupby_node, GroupByStmt *&stmt,std::vector<AggrFuncExpr*>&agg_exprs);

  static RC create_groupby_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      Expression * expr, GroupByUnit *&groupby_unit);

private:
  std::vector<GroupByUnit *> groupby_units_;
  std::vector<AggrFuncExpr *> agg_exprs_;
};
