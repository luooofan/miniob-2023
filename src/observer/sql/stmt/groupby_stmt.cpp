#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/stmt/groupby_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/stmt/filter_stmt.h"

GroupByStmt::~GroupByStmt()
{
  for (GroupByUnit *unit : groupby_units_) {
    delete unit;
  }
  groupby_units_.clear();
}

RC GroupByStmt::create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const GroupBySqlNode *groupby_node, GroupByStmt *&stmt, std::vector<AggrFuncExpr*>&agg_exprs, std::vector<FieldExpr*>& field_exprs)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  // TODO
  // std::vector<GroupByUnit*> groupby_units;
  // for(Expression * expr: groupby_node->exprs_)
  // {
  //   GroupByUnit *groupby_unit = nullptr;
  //   rc = create_groupby_unit(db, default_table, tables, expr, groupby_unit);
  //   if (rc != RC::SUCCESS) {
  //     LOG_WARN("failed to create groupby unit.");
  //     return rc;
  //   }
  //   ASSERT(nullptr != groupby_unit, "ERROR!");
  //   groupby_units.emplace_back(groupby_unit);
  // }

  // everything alright
  stmt = new GroupByStmt();
  stmt->set_agg_exprs(agg_exprs);
  stmt->set_field_exprs(field_exprs);
  return rc;
}

RC GroupByStmt::create_groupby_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    Expression *expr, GroupByUnit *&groupby_unit)
{
  RC rc = RC::SUCCESS;

  // 这里应该做 check
  groupby_unit = new GroupByUnit;
  groupby_unit->set_expr(expr);
  return rc;
}
