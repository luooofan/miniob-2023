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
// Created by Wangyunlai on 2021/5/14.
//

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "common/log/log.h"
#include "sql/expr/tuple_cell.h"
#include "sql/parser/parse.h"
#include "sql/parser/value.h"
#include "sql/expr/expression.h"
#include "storage/record/record.h"
#include "common/lang/bitmap.h"
#include "sql/stmt/groupby_stmt.h"

class Table;

/**
 * @defgroup Tuple
 * @brief Tuple 元组，表示一行数据，当前返回客户端时使用
 * @details 
 * tuple是一种可以嵌套的数据结构。
 * 比如select t1.a+t2.b from t1, t2;
 * 需要使用下面的结构表示：
 * @code {.cpp}
 *  Project(t1.a+t2.b)
 *        |
 *      Joined
 *      /     \
 *   Row(t1) Row(t2)
 * @endcode
 * 
 */

/**
 * @brief 元组的结构，包含哪些字段(这里成为Cell)，每个字段的说明
 * @ingroup Tuple
 */
class TupleSchema 
{
public:
  void append_cell(const TupleCellSpec &cell)
  {
    cells_.push_back(cell);
  }
  void append_cell(const char *table, const char *field)
  {
    append_cell(TupleCellSpec(table, field));
  }
  void append_cell(const char *alias)
  {
    append_cell(TupleCellSpec(alias));
  }
  int cell_num() const
  {
    return static_cast<int>(cells_.size());
  }
  const TupleCellSpec &cell_at(int i) const
  {
    return cells_[i];
  }

private:
  std::vector<TupleCellSpec> cells_;
};

/**
 * @brief 元组的抽象描述
 * @ingroup Tuple
 */
class Tuple 
{
public:
  Tuple() = default;
  virtual ~Tuple() = default;

  /**
   * @brief 获取元组中的Cell的个数
   * @details 个数应该与tuple_schema一致
   */
  virtual int cell_num() const = 0;

  /**
   * @brief 获取指定位置的Cell
   * 
   * @param index 位置
   * @param[out] cell  返回的Cell
   */
  virtual RC cell_at(int index, Value &cell) const = 0;

  /**
   * @brief 根据cell的描述，获取cell的值
   * 
   * @param spec cell的描述
   * @param[out] cell 返回的cell
   */
  virtual RC find_cell(const TupleCellSpec &spec, Value &cell,int &index) const = 0;


  virtual std::string to_string() const
  {
    std::string str;
    const int cell_num = this->cell_num();
    for (int i = 0; i < cell_num - 1; i++) {
      Value cell;
      cell_at(i, cell);
      str += cell.to_string();
      str += ", ";
    }

    if (cell_num > 0) {
      Value cell;
      cell_at(cell_num - 1, cell);
      str += cell.to_string();
    }
    return str;
  }
};

/**
 * @brief 一行数据的元组
 * @ingroup Tuple
 * @details 直接就是获取表中的一条记录
 */
class RowTuple : public Tuple 
{
public:
  RowTuple() = default;
  virtual ~RowTuple()
  {
    for (FieldExpr *spec : speces_) {
      delete spec;
    }
    speces_.clear();
  }

  void set_record(Record *record)
  {
    this->record_ = record;
    ASSERT(!this->speces_.empty(), "RowTuple speces empty!");
    const FieldMeta* null_field = this->speces_.front()->field().meta();
    ASSERT(nullptr != null_field && CHARS == null_field->type(), "RowTuple get null field failed!");
    bitmap_.init(record->data() + null_field->offset(), this->speces_.size());
  }

  void set_schema(const Table *table)
  {
    ASSERT(nullptr != table, "RowTuple set_schema with a null table");
    table_ = table;
    const std::vector<FieldMeta> *fields = table_->table_meta().field_metas();
    this->speces_.clear();//有的算子会 反复open close 
    this->speces_.reserve(fields->size());
    for (const FieldMeta &field : *fields) {
      speces_.push_back(new FieldExpr(table, &field));
    }
  }

  int cell_num() const override
  {
    return speces_.size();
  }

  RC cell_at(int index, Value &cell) const override
  {
    RC rc = RC::SUCCESS;
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }

    if (bitmap_.get_bit(index)) {
      cell.set_null();
    } else {
      FieldExpr *field_expr = speces_[index];
      const FieldMeta *field_meta = field_expr->field().meta();
      if (TEXTS == field_meta->type()) {
        cell.set_type(CHARS);
        int64_t offset = *(int64_t*)(record_->data() + field_meta->offset());
        int64_t length = *(int64_t*)(record_->data() + field_meta->offset() + sizeof(int64_t));
        char *text = (char*)malloc(length);
        rc = table_->read_text(offset, length, text);
        if (RC::SUCCESS != rc) {
          LOG_WARN("Failed to read text from table, rc=%s", strrc(rc));
          return rc;
        }
        cell.set_data(text, length);
        free(text);
      } else {
        cell.set_type(field_meta->type());
        cell.set_data(this->record_->data() + field_meta->offset(), field_meta->len());
      }
    }
    return RC::SUCCESS;
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell,int &index) const override
  {
    const char *table_name = spec.table_name();
    const char *field_name = spec.field_name();
    if (0 != strcmp(table_name, table_->name())) {
      return RC::NOTFOUND;
    }

    for (size_t i = 0; i < speces_.size(); ++i) {
      const FieldExpr *field_expr = speces_[i];
      const Field &field = field_expr->field();
      if (0 == strcmp(field_name, field.field_name())) {
        index = i;
        return cell_at(i, cell);
      }
    }
    return RC::NOTFOUND;
  }

#if 0
  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }
#endif

  Record &record()
  {
    return *record_;
  }

  const Record &record() const
  {
    return *record_;
  }

private:
  Record *record_ = nullptr;
  common::Bitmap bitmap_;
  const Table *table_ = nullptr;
  std::vector<FieldExpr *> speces_;
};

/**
 * @brief 从一行数据中，选择部分字段组成的元组，也就是投影操作
 * @ingroup Tuple
 * @details 一般在select语句中使用。
 * 投影也可以是很复杂的操作，比如某些字段需要做类型转换、重命名、表达式运算、函数计算等。
 * 当前的实现是比较简单的，只是选择部分字段，不做任何其他操作。
 */
class ProjectTuple : public Tuple 
{
public:
  ProjectTuple() = default;
  virtual ~ProjectTuple() { exprs_.clear(); }

  void set_tuple(Tuple *tuple)
  {
    this->tuple_ = tuple;
  }

  void add_expr(std::unique_ptr<Expression> expr)
  {
    exprs_.emplace_back(std::move(expr));
  }
  int cell_num() const override
  {
    return exprs_.size();
  }

  RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(exprs_.size())) {
      return RC::INTERNAL;
    }
    if (tuple_ == nullptr) {
      return RC::INTERNAL;
    }

    // 原本这里会根据 tuple cell spec 去 tuple_ 里 find cell
    // 现在这个逻辑是在 FieldExpr 的 get_value 里面
    return exprs_[index]->get_value(*tuple_, cell);
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell,int &index) const override
  {
    return tuple_->find_cell(spec, cell,index);//TODO 应该不会走到这里
  }

  const std::vector<std::unique_ptr<Expression>>& expressions() const
  {
    return exprs_;
  }
#if 0
  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::NOTFOUND;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }
private:
  std::vector<TupleCellSpec *> speces_;
#endif
private:
  std::vector<std::unique_ptr<Expression>> exprs_;
  Tuple *tuple_ = nullptr;
};

class ExpressionTuple : public Tuple 
{
public:
  ExpressionTuple(std::vector<std::unique_ptr<Expression>> &expressions)
    : expressions_(expressions)
  {
  }
  
  virtual ~ExpressionTuple()
  {
  }

  int cell_num() const override
  {
    return expressions_.size();
  }

  RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(expressions_.size())) {
      return RC::INTERNAL;
    }

    const Expression *expr = expressions_[index].get();
    return expr->try_get_value(cell);
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell,int &index) const override
  {
    for (const std::unique_ptr<Expression> &expr : expressions_) {
      if (0 == strcmp(spec.alias(), expr->name().c_str())) {
        return expr->try_get_value(cell);
      }
    }
    return RC::NOTFOUND;
  }

private:
  const std::vector<std::unique_ptr<Expression>> &expressions_;
};

/**
 * @brief 一些常量值组成的Tuple
 * @ingroup Tuple
 */
class ValueListTuple : public Tuple 
{
public:
  ValueListTuple() = default;
  virtual ~ValueListTuple() = default;

  void set_cells(const std::vector<Value> &cells)
  {
    cells_ = cells;
  }

  virtual int cell_num() const override
  {
    return static_cast<int>(cells_.size());
  }

  virtual RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= cell_num()) {
      return RC::NOTFOUND;
    }

    cell = cells_[index];
    return RC::SUCCESS;
  }

  virtual RC find_cell(const TupleCellSpec &spec, Value &cell,int &index) const override
  {
    return RC::INTERNAL;
  }

private:
  std::vector<Value> cells_;
};

/**
 * @brief 将两个tuple合并为一个tuple
 * @ingroup Tuple
 * @details 在join算子中使用
 */
class JoinedTuple : public Tuple 
{
public:
  JoinedTuple() = default;
  JoinedTuple(Tuple* left, Tuple* right) : left_(left), right_(right) {}
  virtual ~JoinedTuple() = default;

  void set_left(Tuple *left)
  {
    left_ = left;
  }
  void set_right(Tuple *right)
  {
    right_ = right;
  }

  int cell_num() const override
  {
    return left_->cell_num() + right_->cell_num();
  }

  RC cell_at(int index, Value &value) const override
  {
    const int left_cell_num = left_->cell_num();
    if (index > 0 && index < left_cell_num) {
      return left_->cell_at(index, value);
    }

    if (index >= left_cell_num && index < left_cell_num + right_->cell_num()) {
      return right_->cell_at(index - left_cell_num, value);
    }

    return RC::NOTFOUND;
  }

  RC find_cell(const TupleCellSpec &spec, Value &value,int &index) const override
  {
    RC rc = left_->find_cell(spec, value,index);
    if (rc == RC::SUCCESS || rc != RC::NOTFOUND) {
      return rc;
    }
 
    rc = right_->find_cell(spec, value,index);
    if( rc != RC::SUCCESS)
    {
      return rc;
    }
    index += left_->cell_num();
    return rc;
  }
private:
  Tuple *left_ = nullptr;
  Tuple *right_ = nullptr;
};


class GroupTuple : public Tuple {
public:
  GroupTuple() = default;
  virtual ~GroupTuple() = default;

  void set_tuple(Tuple *tuple)
  {
    this->tuple_ = tuple;
  }

  int cell_num() const override
  {
    return tuple_->cell_num();
  }

  RC cell_at(int index, Value &cell) const override
  {
    if (tuple_ == nullptr) {
      return RC::INTERNAL;
    }
    //field_results_ 数组的下标从0开始，aggr_results_ 的下标从 field_results_.size() 开始
    if (index < 0 || index >= aggr_results_.size() + field_results_.size()) {
      return RC::NOTFOUND;
    }

    if(index < field_results_.size()){
      cell = field_results_[index].result();
    }else {
      cell = aggr_results_[index - field_results_.size()].result();
    }
    return RC::SUCCESS;
  }

  size_t find_agg_index_by_name(std::string expr_name) const
  {
    for(size_t i = 0; i < aggr_results_.size(); ++i) {
      if(expr_name == aggr_results_[i].name()) {
        return i;
      }
    }
    return -1;
  }
  // RC find_cell(std::string expr_name,Value &cell,int &index) const
  // {
  //   index = find_agg_index_by_name(expr_name);
  //   if (index < 0 || index >= aggr_results_.size()) {
  //     return RC::NOTFOUND;
  //   }
  //   cell = aggr_results_[index].result();
  //   ////因为 GroupTuple 有两个存放结果的 vector ，所以给 aggr_results_ 返回的 index 加上一个偏移量
  //   index += field_results_.size();
  //   return RC::SUCCESS;
  // }
  RC find_cell(const TupleCellSpec &spec, Value &cell,int& index) const override
  {
    // 先从字段表达式里面找
    for (size_t i = 0; i < field_results_.size(); ++i) {
      const FieldExpr &expr = *field_results_[i].expr();
      if (std::string(expr.field_name()) == std::string(spec.field_name()) && 
          std::string(expr.table_name()) == std::string(spec.table_name()) ) {
        cell = field_results_[i].result();
        index = i;
        LOG_INFO("Field is found in field_exprs");
        return RC::SUCCESS;
      }
    }
    // 找不到再根据别名从聚集函数表达式里面找
    //return find_cell(std::string(spec.alias()), cell,index);
    index = find_agg_index_by_name(std::string(spec.alias()));
    if (index < 0 || index >= aggr_results_.size()) {
      return RC::NOTFOUND;
    }
    cell = aggr_results_[index].result();
    ////因为 GroupTuple 有两个存放结果的 vector ，所以给 aggr_results_ 返回的 index 加上一个偏移量
    index += field_results_.size();
    return RC::SUCCESS;
  }

  void init(std::vector<std::unique_ptr<AggrFuncExpr>> &&aggr_exprs,
      std::vector<std::unique_ptr<FieldExpr>> &&field_exprs)
  {
    aggr_results_.resize(aggr_exprs.size());
    for (size_t i = 0; i < aggr_exprs.size(); ++i) {
      aggr_results_[i].set_expr(std::move(aggr_exprs[i]));
    }
    aggr_exprs.clear();

    field_results_.resize(field_exprs.size());
    for (size_t i = 0; i < field_exprs.size(); ++i) {
      field_results_[i].set_expr(std::move(field_exprs[i]));
    }
    field_exprs.clear();
  }
  void reset()
  {
    for (auto& res : aggr_results_) {
      res.reset();
    }
    for (auto& res : field_results_) {
      res.reset();
    }
  }
  void do_aggregate_first()
  {
    // first row in current group
    for (size_t i = 0; i < aggr_results_.size(); ++i) {
      aggr_results_[i].init(*tuple_);
    }
    // do this only at "first"
    for (size_t i = 0; i < field_results_.size(); ++i) {
      field_results_[i].init(*tuple_);
    }
  }
  void do_aggregate()
  {
    // other rows in current group
    for (auto& ar : aggr_results_) {
      ar.advance(*tuple_);
    }
  }
  void do_aggregate_done()
  {
    // set result for current group
    for (auto& ar : aggr_results_) {
      ar.finish();
    }
  }

  class AggrExprResults {
  public:
    // 每个 group 的第一行调用一次 init
    void init(const Tuple& tuple)
    {
      // 1. reset
      count_ = 0;
      all_null_ = true;
      // 2. count(1) count(*) count(1+1)
      if (expr_->is_count_constexpr()) {
        // 不能跳过 null 这种情况下可以直接递增 count_
        count_ = 1;
        return;
      }
      // 3. get current value and set result_
      expr_->get_param()->get_value(tuple, result_);
      // 4. ignore null
      if (!result_.is_null()) {
        count_ = 1;
        all_null_ = false;
      }
      return;
    }
    // 每个 group 进行中间结果的计算
    void advance(const Tuple& tuple)
    {
      // 1. count(1) count(*) count(1+1)
      if (expr_->is_count_constexpr()) {
        count_++;
        return;
      }
      // 2. get current value
      Value temp;
      expr_->get_param()->get_value(tuple, temp);
      // 3. ignore null
      if (temp.is_null()) { // 直接跳过
        return;
      }
      // 4. update status
      count_++;
      all_null_ = false;
      // 5. init 的时候拿到的是 null
      if (result_.is_null()) {
        result_ = temp;
        return;
      }
      // 6. do aggr calc
      switch (expr_->get_aggr_func_type()) {
        case AggrFuncType::AGG_COUNT: {
          // do nothing
        } break;
        case AggrFuncType::AGG_AVG:
        case AggrFuncType::AGG_SUM: {
          result_.add(temp);
        } break;
        case AggrFuncType::AGG_MAX: {
          if (result_ < temp) {
            result_ = temp;
          }
        } break;
        case AggrFuncType::AGG_MIN: {
          if (result_ > temp) {
            result_ = temp;
          }
        } break;
        default: {
          LOG_ERROR("Unsupported AggrFuncType");
        } break;
      }
    }
    // 每个 group 迭代完之后计算最终结果
    void finish()
    {
      // 1. count(*) count(1) count(1+1) count(id)
      if (expr_->get_aggr_func_type() == AGG_COUNT) {
        result_.set_int(count_);
        return;
      }
      // 2. all null
      if (all_null_) {
        result_.set_null();
        return;
      }
      // 3. other situation
      switch (expr_->get_aggr_func_type()) {
        case AggrFuncType::AGG_COUNT: {
          result_.set_int(count_);
        } break;
        case AggrFuncType::AGG_AVG: {
          result_.div(Value(count_));
        } break;
        default: {
          // do nothing for other type
        } break;
      }
    }
    void reset()
    {
      count_ = 0;
      all_null_ = true;
    }
    const std::string name() const
    {
      return expr_->name();
    }
    const Value& result() const
    {
      return result_;
    }
    void set_expr(std::unique_ptr<AggrFuncExpr> expr)
    {
      expr_ = std::move(expr);
    }
  private:
    std::unique_ptr<AggrFuncExpr> expr_;
    Value result_;
    int count_ = 0;
    bool all_null_ = true;
  };

  class FieldExprResults {
  public:
    // only called when first row in a new group. no need advance
    void init(const Tuple& tuple)
    {
      expr_->get_value(tuple, result_); // do nothing for null
    }
    void reset() {}
    const Value& result() const
    {
      return result_;
    }
    void set_expr(std::unique_ptr<FieldExpr> expr)
    {
      expr_ = std::move(expr);
    }
    const std::unique_ptr<FieldExpr> &expr() const
    {
      return expr_;
    }
  private:
    std::unique_ptr<FieldExpr> expr_;
    Value result_;
  };

private:
  int count_ = 0;
  std::vector<AggrExprResults> aggr_results_;
  std::vector<FieldExprResults> field_results_;
  Tuple *tuple_ = nullptr;
};

class EmptyTuple : public Tuple 
{
public:
  EmptyTuple() = default;
  virtual ~EmptyTuple() = default;

  int cell_num() const { return 0; }

  RC cell_at(int index, Value &cell) const { return RC::INVALID_ARGUMENT; }

  RC find_cell(const TupleCellSpec &spec, Value &cell,int &index) const { return RC::INVALID_ARGUMENT; }
};


/**
 * @brief 一些常量值组成的Tuple,用于 orderby 算子中
 * @ingroup Tuple
 */
class SplicedTuple : public Tuple 
{
public:
  SplicedTuple() = default;
  virtual ~SplicedTuple() = default;

  void set_cells(const std::vector<Value>* cells)
  {
    cells_ = cells;
  }

  virtual int cell_num() const override
  {
    return static_cast<int>((*cells_).size());
  }

  virtual RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= cell_num()) {
      return RC::NOTFOUND;
    }

    cell = (*cells_)[index];
    return RC::SUCCESS;
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell,int & index) const override
  {
    // 先从字段表达式里面找
    for (size_t i = 0; i < exprs_.size(); ++i) {
      if(exprs_[i]->type() == ExprType::FIELD){
        const FieldExpr * expr =static_cast<FieldExpr*>(exprs_[i].get());
        if (std::string(expr->field_name()) == std::string(spec.field_name()) && 
            std::string(expr->table_name()) == std::string(spec.table_name()) ) {
            cell = (*cells_)[i];
            index = i;
          LOG_INFO("Field is found in field_exprs");
          return RC::SUCCESS;
        }
      }else if(exprs_[i]->type() == ExprType::AGGRFUNCTION){
        if(spec.alias() == exprs_[i]->name()){
          cell = (*cells_)[i];
          index = i;
          return RC::SUCCESS;
        }
      }else{
        LOG_WARN("find cell in SplicedTuple error!");
        return RC::INTERNAL;
      }
    }
    LOG_WARN(" not find cell in SplicedTuple ");
    return RC::NOTFOUND;
  }

  RC init(std::vector<std::unique_ptr<Expression>> &&exprs)
  {
    exprs_ = std::move(exprs);
    return RC::SUCCESS;
  }
  
  std::vector<std::unique_ptr<Expression>> &exprs()
  {
    return exprs_;
  }

private:
  const std::vector<Value> *cells_ = nullptr;
  //在 create order by stmt 之前提取的  select clause 后的 field_expr (非a gg_expr 中的)和 agg_expr
  std::vector<std::unique_ptr<Expression>> exprs_;
};