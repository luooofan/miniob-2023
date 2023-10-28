#pragma once

#include "sql/operator/logical_operator.h"
#include "storage/field/field.h"

class View;

/**
 * @brief 表示从表中获取数据的算子
 * @details 比如使用全表扫描、通过索引获取数据等
 * @ingroup LogicalOperator
 */
class ViewGetLogicalOperator : public LogicalOperator
{
public:
  ViewGetLogicalOperator(View *table, const std::vector<Field> &fields, bool readonly);
  virtual ~ViewGetLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::VIEW_GET;
  }

  View *view() const  { return table_; }
  bool readonly() const { return readonly_; }

  void set_predicates(std::vector<std::unique_ptr<Expression>> &&exprs);
  std::vector<std::unique_ptr<Expression>> &predicates()
  {
    return predicates_;
  }

private:
  View *table_ = nullptr;
  std::vector<Field> fields_;
  bool readonly_ = false;

  // 与当前表相关的过滤操作，可以尝试在遍历数据时执行
  // 这里的表达式都是比较简单的比较运算，并且左右两边都是取字段表达式或值表达式
  // 不包含复杂的表达式运算，比如加减乘除、或者conjunction expression
  // 如果有多个表达式，他们的关系都是 AND
  std::vector<std::unique_ptr<Expression>> predicates_;
};