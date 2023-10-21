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
    const GroupBySqlNode *groupby_node, GroupByStmt *&stmt,std::vector<AggrFuncExpr*>&agg_exprs)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;
  if(groupby_node == nullptr)
  {
    return rc;
  }
  GroupByStmt *tmp_stmt = new GroupByStmt();
  for(Expression * expr:groupby_node->exprs_)
  {
    GroupByUnit *groupby_unit = nullptr;
    rc = create_groupby_unit(db,default_table,tables,expr,groupby_unit);
    if (rc != RC::SUCCESS) 
    {
      delete tmp_stmt;
      LOG_WARN("failed to create groupby unit.");
      return rc;
    }
    tmp_stmt->groupby_units_.push_back(groupby_unit);
  }
  tmp_stmt->set_agg_exprs(agg_exprs);
  stmt = tmp_stmt;
  return rc;
}

RC GroupByStmt::create_groupby_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    Expression *expr, GroupByUnit *&groupby_unit)
{
    RC rc = RC::SUCCESS;

    groupby_unit = new GroupByUnit;
    groupby_unit->set_expr(expr);
    return rc;
}
