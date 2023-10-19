/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sql/optimizer/predicate_merge_rewriter.h"
#include "sql/operator/logical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/expr/expression.h"

RC PredicateMergeRewriter::rewrite(std::unique_ptr<LogicalOperator> &oper, bool &change_made)
{
  RC rc = RC::SUCCESS;
  if (oper->type() != LogicalOperatorType::PREDICATE) {
    return rc;
  }

  if (oper->children().size() != 1) {
    return rc;
  }

  std::unique_ptr<LogicalOperator> &child_oper = oper->children().front();
  if (child_oper->type() != LogicalOperatorType::PREDICATE) {
    return rc;
  }

  auto sub_predicate_oper = static_cast<PredicateLogicalOperator *>(child_oper.get());

  // predicate logical oper 里一定是只有一个孩子: 要么是 conjunction expr 要么是 comparison expr
  std::vector<std::unique_ptr<Expression>> &predicate_oper_exprs = oper->expressions();
  if (predicate_oper_exprs.size() != 1) {
    return rc;
  }
  std::vector<std::unique_ptr<Expression>> &sub_predicate_oper_exprs = sub_predicate_oper->expressions();
  if (sub_predicate_oper_exprs.size() != 1) {
    return rc;
  }

  std::unique_ptr<Expression> &predicate_expr = predicate_oper_exprs.front();
  std::unique_ptr<Expression> &sub_predicate_expr = sub_predicate_oper_exprs.front();

  rc = merge(predicate_expr, sub_predicate_expr);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to merge cmpare exprs. rc=%s", strrc(rc));
    return rc;
  }

  ASSERT(!predicate_expr, "ERROR!");
  // 所有的表达式都合并到了下层算子
  // 这个predicate operator其实就可以不要了。但是这里没办法删除，弄一个空的表达式吧
  LOG_TRACE("all expressions of predicate operator were merged to child predicate operator, then make a fake one");

  Value value((bool)true);
  predicate_expr = std::unique_ptr<Expression>(new ValueExpr(value));

  change_made = true;
  return rc;
}


RC PredicateMergeRewriter::merge(std::unique_ptr<Expression>& source, std::unique_ptr<Expression>& target)
{
  RC rc = RC::SUCCESS;
    // unique_ptr<ConjunctionExpr> conjunction_expr(new ConjunctionExpr(ConjunctionExpr::Type::AND, std::move(cmp_exprs)));
  std::vector<std::unique_ptr<Expression>> cmp_exprs;
  auto extract_exprs = [&cmp_exprs](std::unique_ptr<Expression>& expr) {
    if (expr->type() == ExprType::CONJUNCTION) {
      ConjunctionExpr *conjunction_expr = static_cast<ConjunctionExpr *>(expr.get());
      ASSERT(conjunction_expr->conjunction_type() != ConjunctionExpr::Type::OR, "ERROR!");
      std::vector<std::unique_ptr<Expression>> &child_exprs = conjunction_expr->children();
      cmp_exprs.insert(cmp_exprs.end(), 
                      std::make_move_iterator(child_exprs.begin()),
                      std::make_move_iterator(child_exprs.end()));
      expr.reset();
    } else if (expr->type() == ExprType::COMPARISON) {
      cmp_exprs.emplace_back(std::move(expr));
    }
    return;
  };

  // 先把所有表达式都提取到 cmp_exprs 中
  // TODO(wbj) 这里没有考虑 OR. 不能出现有 OR 的 ConjunctionExpr
  extract_exprs(source);
  extract_exprs(target);

  // 新分配一个 ConjunctionExpr
  std::unique_ptr<Expression> new_expr = std::make_unique<ConjunctionExpr>(ConjunctionExpr::Type::AND, std::move(cmp_exprs));
  std::swap(target, new_expr);
  return rc;
}