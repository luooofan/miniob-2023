#include "sql/stmt/create_view_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "storage/db/db.h"
#include "storage/field/field.h"
#include "event/sql_debug.h"

CreateViewStmt::CreateViewStmt(bool allow_write, 
                              const std::string view_name,  
                              const std::vector<AttrInfoSqlNode> attr_infos, 
                              std::vector<Field> map_fields, 
                              SelectSqlNode *select_sql)
  : view_name_(std::move(view_name)), allow_write_(allow_write), 
    attr_infos_(std::move(attr_infos)), map_fields_(std::move(map_fields))
{
  select_sql_.deep_copy(*select_sql);
}

RC CreateViewStmt::create(Db *db, const CreateViewSqlNode &create_view, Stmt *&stmt, SelectSqlNode &select_sql)
{
  RC rc = RC::SUCCESS;

  if (select_sql.project_exprs.empty()) {
    LOG_ERROR("select_stmt syntax error in create_view_stmt");
    return RC::SQL_SYNTAX;
  }
  if (!create_view.col_names.empty() && create_view.col_names.size() != select_sql.project_exprs.size()) {
    LOG_WARN("output columns of select_stmt mismatch with view definetion");
    return RC::INVALID_ARGUMENT;
  }

  Stmt *sub_stmt = nullptr;
  SelectSqlNode sql_back;
  sql_back.deep_copy(select_sql);
  rc = SelectStmt::create(db, select_sql, sub_stmt);
  if (RC::SUCCESS != rc) {
    LOG_WARN("failed to create select_stmt when create_table_select, rc=%s", strrc(rc));
    return rc;
  }

  bool allow_write = true;
  std::vector<Field> map_fields;
  std::vector<AttrInfoSqlNode> attr_infos;
  SelectStmt *select_stmt = static_cast<SelectStmt*>(sub_stmt);
  for (std::unique_ptr<Expression> &attr_expr : select_stmt->projects()) {
    Field map_field;
    AttrInfoSqlNode attr_info;
    if (0 != attr_expr->alias().length()) {
      size_t pos = attr_expr->alias().find('.');
      if (std::string::npos == pos) {
        attr_info.name = attr_expr->alias();
      } else {
        attr_info.name = attr_expr->alias().substr(pos + 1);
      }
    } else {
      attr_info.name = attr_expr->name();
    }
    attr_info.type = attr_expr->value_type();

    if (ExprType::FIELD == attr_expr->type()) {
      FieldExpr *field_expr = dynamic_cast<FieldExpr*>(attr_expr.get());
      map_field = field_expr->field();
      attr_info.length = field_expr->get_field_meta().len();
      attr_info.nullable = field_expr->get_field_meta().nullable();
    } else {
      allow_write = false;
      if (ExprType::VALUE == attr_expr->type()) {
        ValueExpr *value_expr = dynamic_cast<ValueExpr*>(attr_expr.get());
        attr_info.length = value_expr->get_value().length();
      } else {
        attr_info.length = ATTR_TYPE_LENGTH[attr_expr->value_type()];
      }
      // 遍历子表达式，有nullable的FieldExpr时，才允许为NULL
      bool nullable = false;
      auto check_expr_nullable = [&nullable](Expression *expr) {
        if (ExprType::FIELD == expr->type()) {
          FieldMeta field = static_cast<FieldExpr*>(expr)->get_field_meta();
          if (field.nullable()) {
            nullable = true;
          } 
        }
      };
      attr_expr->traverse(check_expr_nullable);
      attr_info.nullable = nullable;
    }
    map_fields.emplace_back(map_field);
    attr_infos.emplace_back(attr_info);
  }

  if (!create_view.col_names.empty()) {
    for (size_t i = 0; i < create_view.col_names.size(); i++) {
      attr_infos[i].name = create_view.col_names[i];
    }
  }

  stmt = new CreateViewStmt(allow_write, std::move(create_view.view_name), 
                            std::move(attr_infos), std::move(map_fields), &sql_back);
  
  sql_debug("create table statement: table name %s", create_view.view_name.c_str());
  delete sub_stmt;
  return RC::SUCCESS;
}
