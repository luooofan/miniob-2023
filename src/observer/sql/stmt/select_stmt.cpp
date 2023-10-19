/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "session/session.h"
SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}


static void wildcard_fields(Table *table, std::vector<Expression *> &projects,bool is_single_table)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    if (table_meta.field(i)->visible()) {
      FieldExpr *tmp = new FieldExpr(table, table_meta.field(i));
      if(is_single_table)
      {
        std::string field_name(tmp->field_name());
        tmp->set_name(field_name);
      } 
      else
      {
        std::string alis;
        std::string table_name(tmp->table_name());
        std::string field_name(tmp->field_name());
        tmp->set_name(table_name + "." + field_name);
      }  
      projects.emplace_back(tmp);
    }
  }
}
RC SelectStmt::create(Db *db, const SelectSqlNode &select_sql, Stmt *&stmt,SQLStageEvent *sql_event)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }
  SqlResult *sql_result = sql_event->session_event()->sql_result();
  TupleSchema schema;//报错输出的表头
  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;
  for (size_t i = 0; i < select_sql.relations.size(); i++) {
    const char *table_name = select_sql.relations[i].c_str();
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_name, table));
  }
  bool is_single_table = (tables.size() == 1) ;
  std::vector<Expression *> projects;
  for (int i = static_cast<int>(select_sql.project_exprs.size()) - 1; i >= 0; i--)
  {
    RC rc = RC::SUCCESS;
    Expression* expr = select_sql.project_exprs[i];//将sqlNode的表达式转移到SelectStmt中
    //单独处理 select 后跟*的情况
    if(expr->type()== ExprType::FIELD)
    {
      bool do_wildcard_fields = false;
      FieldExpr *field_expr = static_cast<FieldExpr*>(expr);
      if (common::is_blank(field_expr->get_table_name().c_str()) &&
                0 == strcmp(field_expr->get_field_name().c_str(), "*")) //表名为空---> "*"
      {
          for (Table *table : tables) 
          {
            do_wildcard_fields = true;
            wildcard_fields(table, projects,is_single_table);
          }
      }
      else if(!common::is_blank(field_expr->get_table_name().c_str())) //表名不为空
      {
          const char *table_name = field_expr->get_table_name().c_str();
          const char*field_name = field_expr->get_field_name().c_str();

          if (0 == strcmp(table_name, "*")) //表名为 *,列名必须也为 *
          {
            if (0 != strcmp(field_name, "*"))  //*.*
            {
              LOG_WARN("invalid field name while table is *. attr=%s", field_name);
              return RC::SCHEMA_FIELD_MISSING;
            }
            for (Table *table : tables) 
            {
              do_wildcard_fields = true;
              wildcard_fields(table, projects,is_single_table);
            }
          } //表名t1
          else 
          {
            auto iter = table_map.find(table_name);
            if (iter == table_map.end()) 
            {
              LOG_WARN("no such table in from list: %s", table_name);
              return RC::SCHEMA_FIELD_MISSING;
            }
            //t1.*
            Table *table = iter->second;
            if (0 == strcmp(field_name, "*")) 
            {
              do_wildcard_fields = true;
              wildcard_fields(table, projects,is_single_table);
            } 
          }
      }

      if(!do_wildcard_fields)
      {
        Expression *res_expr = NULL;
        rc =expr->create_expression(table_map,tables,db,res_expr);
        projects.emplace_back(res_expr);
        if(rc != RC::SUCCESS)
        {
          LOG_INFO("expr->create_expression error!");
          return rc;
        }
      }
    }
    else
    {
      Expression *res_expr = NULL;
      rc =expr->create_expression(table_map,tables,db,res_expr);
      projects.emplace_back(res_expr);//将生成的新的表达式加入到 projects 中
      if(rc != RC::SUCCESS)
      {
        LOG_INFO("expr->create_expression error!");
        return rc;
      }
    }
  }//end for

 //LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), query_fields.size());

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
  }
  bool with_table_name = tables.size() > 1;
  for(Expression * expr : projects)
  {
    schema.append_cell(expr->name().c_str());
  }
  sql_result->set_tuple_schema(schema);
  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db,
      default_table,
      &table_map,
      select_sql.conditions.data(),
      static_cast<int>(select_sql.conditions.size()),
      filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // everything alright
  SelectStmt *select_stmt = new SelectStmt();
  // TODO add expression copy
  select_stmt->tables_.swap(tables);
  select_stmt->projects_.swap(projects);
  //select_stmt->query_fields_.swap(query_fields);
  select_stmt->filter_stmt_ = filter_stmt;
  stmt = select_stmt;
  return RC::SUCCESS;
}
