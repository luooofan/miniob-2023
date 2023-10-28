#include "sql/operator/view_get_logical_operator.h"

ViewGetLogicalOperator::ViewGetLogicalOperator(View *table, const std::vector<Field> &fields, bool readonly)
    : table_(table), fields_(fields), readonly_(readonly)
{}

void ViewGetLogicalOperator::set_predicates(std::vector<std::unique_ptr<Expression>> &&exprs)
{
  predicates_ = std::move(exprs);
}