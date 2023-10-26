#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/stmt/groupby_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/stmt/filter_stmt.h"

RC GroupByStmt::create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const std::vector<Expression*>& groupby_expr, GroupByStmt *&stmt,
    std::vector<std::unique_ptr<AggrFuncExpr>> &&agg_exprs,
    std::vector<std::unique_ptr<FieldExpr>> &&field_exprs)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  std::vector<std::unique_ptr<Expression>> groupby_fields;
  for(auto expr : groupby_expr)
  {
    groupby_fields.emplace_back(expr);
  }
  // everything alright
  stmt = new GroupByStmt();
  stmt->set_agg_exprs(std::move(agg_exprs));
  stmt->set_field_exprs(std::move(field_exprs));
  stmt->set_groupby_fields(std::move(groupby_fields));
  return rc;
}