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
// Created by Wangyunlai on 2022/5/22.
//

#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/defer.h"
#include "common/lang/string.h"
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

RC FilterStmt::create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    Expression *condition, FilterStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;
  if (condition == nullptr) {
    return rc;
  }

  auto check_condition_expr = [&db, &tables, &default_table](Expression *expr) {
    // TODO 聚集函数会出现在 having condition 中
    if (expr->type() == ExprType::SYSFUNCTION) {
      SysFuncExpr* sysfunc_expr = static_cast<SysFuncExpr*>(expr);
      return sysfunc_expr->check_param_type_and_number();
    }
    if (expr->type() == ExprType::FIELD) {
      FieldExpr* field_expr = static_cast<FieldExpr*>(expr);
      // default_table 是有效的 所以 table* vector 可以为空
      // 条件表达式检查时可以不用 table_alias_map 因为不需要设置它的 alias
      return field_expr->check_field(*tables, {}, default_table, {});
    }
    if (expr->type() == ExprType::SUBQUERY) {
      // 条件表达式里才有子查询
      SubQueryExpr* subquery_expr = static_cast<SubQueryExpr*>(expr);
      return subquery_expr->generate_select_stmt(db, *tables);
    }
    if (expr->type() == ExprType::COMPARISON) {
      ComparisonExpr* cmp_expr = static_cast<ComparisonExpr*>(expr);
      CompOp comp = cmp_expr->comp();
      if (comp < EQUAL_TO || comp >= NO_OP) {
        LOG_WARN("invalid compare operator : %d", comp);
        return RC::INVALID_ARGUMENT;
      }
      return RC::SUCCESS;
    }
    return RC::SUCCESS;
  };
  if (RC rc = condition->traverse_check(check_condition_expr); RC::SUCCESS != rc) {
    return rc;
  }

  FilterStmt *tmp_stmt = new FilterStmt();
  tmp_stmt->condition_ = std::unique_ptr<Expression>(condition);

  stmt = tmp_stmt;
  return rc;
}
