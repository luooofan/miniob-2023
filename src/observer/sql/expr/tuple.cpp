#include "sql/expr/tuple.h"
#include "sql/parser/parse_defs.h"

void GroupTuple::do_aggregate_first()
{
  assert(aggr_results_.size() == aggr_exprs_.size());
  assert(aggr_exprs_.size() == all_null_.size());
  assert(aggr_exprs_.size() == counts_.size());
  assert(field_results_.size() == field_exprs_.size());
  // first row in current group
  for (size_t i = 0; i < all_null_.size(); ++i) {
    all_null_[i] = true;
    counts_[i] = 0;
  }
  for (size_t i = 0; i < aggr_exprs_.size(); ++i) {
        const AggrFuncExpr *expr = aggr_exprs_[i];
        expr->get_param()->get_value(*tuple_, aggr_results_[i]);//从当前tuple中取出相应的一列
        if (!aggr_results_[i].is_null()) {
          all_null_[i] = false;
          counts_[i]++;
        }
  }
  for (size_t i = 0; i < field_exprs_.size(); ++i) {
    field_exprs_[i]->get_value(*tuple_, field_results_[i]);
  }
}

void GroupTuple::do_aggregate()
{
  Value tmp;
  for (size_t i = 0; i < aggr_exprs_.size(); ++i) {
    const AggrFuncExpr *expr = aggr_exprs_[i];
    expr->get_param()->get_value(*tuple_, tmp);
      if (tmp.is_null()) {  // cannot do any aggregate for NULL
        continue;
      }
    all_null_[i] = false;
    counts_[i]++;
    if (AggrFuncType::AGG_COUNT == expr->get_aggr_func_type()) {
      continue;
    }
    // NOTE: aggr_results_[i] maybe null. tmp is not null
    if (aggr_results_[i].is_null()) {
      aggr_results_[i] = tmp;
    }
    switch (expr->get_aggr_func_type()) {
      case AggrFuncType::AGG_MIN:
        aggr_results_[i] = Value::min(aggr_results_[i], tmp);
        break;
      case AggrFuncType::AGG_MAX:
        aggr_results_[i] = Value::max(aggr_results_[i], tmp);
        break;
      case AggrFuncType::AGG_SUM:
      case AggrFuncType::AGG_AVG:
        aggr_results_[i] = Value::add(aggr_results_[i], tmp);
        break;
      default:
        LOG_ERROR("Unsupported AggrFuncType");
        break;
    }
  }
}

void GroupTuple::do_aggregate_done()
{
  for (size_t i = 0; i < aggr_exprs_.size(); ++i) {
    const AggrFuncExpr *expr = aggr_exprs_[i];
    Value &res = aggr_results_[i];
    // if all null, result is null
    if (all_null_[i] && AggrFuncType::AGG_COUNT != expr->get_aggr_func_type()) {
      aggr_results_[i].set_null();
      continue;
    }
    switch (expr->get_aggr_func_type()) {
      case AggrFuncType::AGG_COUNT: {
        res.set_int(counts_[i]);
        break;
      }
      case AggrFuncType::AGG_AVG: {
        Value tmp;
        tmp.set_int(counts_[i]);
        res = Value::div(res,tmp);
        break;
      }
      default:
        break;
    }
  }
}
