#include "sql/stmt/show_index_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

RC ShowIndexStmt::create(Db *db, const ShowIndexSqlNode &show_index_sql,  Stmt *&stmt)
{
  RC rc = RC::SUCCESS;
  const char *table_name = show_index_sql.relation_name.c_str();

  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p", db, table_name);
    rc = RC::INVALID_ARGUMENT;
  } else if (db->find_table(show_index_sql.relation_name.c_str()) == nullptr) {
    rc = RC::SCHEMA_TABLE_NOT_EXIST;
  } else {
    stmt = new ShowIndexStmt(table_name);
  }
  
  return rc;
}