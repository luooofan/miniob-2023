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
// Created by Wangyunlai on 2022/07/05.
//

#pragma once

#include <string.h>
#include <memory>
#include <string>

#include "storage/field/field.h"
#include "sql/parser/value.h"
#include "common/log/log.h"
#include "storage/db/db.h"

class Tuple;

/**
 * @defgroup Expression
 * @brief 表达式
 */

/**
 * @brief 表达式类型
 * @ingroup Expression
 */
enum class ExprType 
{
  NONE,
  STAR,         ///< 星号，表示所有字段
  FIELD,        ///< 字段。在实际执行时，根据行数据内容提取对应字段的值
  VALUE,        ///< 常量值
  CAST,         ///< 需要做类型转换的表达式
  COMPARISON,   ///< 需要做比较的表达式
  CONJUNCTION,  ///< 多个表达式使用同一种关系(AND或OR)来联结
  ARITHMETIC,   ///< 算术运算
  AGGRFUNCTION, ///< 聚集函数
  SYSFUNCTION,  ///< 系统函数
  SUBQUERY,     ///< 子查询
  EXPRLIST,     ///< 表达式列表
};

/**
 * @brief 表达式的抽象描述
 * @ingroup Expression
 * @details 在SQL的元素中，任何需要得出值的元素都可以使用表达式来描述
 * 比如获取某个字段的值、比较运算、类型转换
 * 当然还有一些当前没有实现的表达式，比如算术运算。
 *
 * 通常表达式的值，是在真实的算子运算过程中，拿到具体的tuple后
 * 才能计算出来真实的值。但是有些表达式可能就表示某一个固定的
 * 值，比如ValueExpr。
 */
class Expression 
{
public:
  Expression() = default;
  virtual ~Expression() = default;

  /**
   * @brief 根据具体的tuple，来计算当前表达式的值。tuple有可能是一个具体某个表的行数据
   */
  // TODO 取消 const，有些表达式需要维护内部状态，比如 FieldExpr 维护 is_first
  virtual RC get_value(const Tuple &tuple, Value &value) const = 0;

  /**
   * @brief 在没有实际运行的情况下，也就是无法获取tuple的情况下，尝试获取表达式的值
   * @details 有些表达式的值是固定的，比如ValueExpr，这种情况下可以直接获取值
   */
  virtual RC try_get_value(Value &value) const
  {
    return RC::UNIMPLENMENT;
  }

  /**
   * @brief 表达式的类型
   * 可以根据表达式类型来转换为具体的子类
   */
  virtual ExprType type() const = 0;

  /**
   * @brief 表达式值的类型
   * @details 一个表达式运算出结果后，只有一个值
   */
  virtual AttrType value_type() const = 0;

  virtual void traverse(const std::function<void(Expression*)>& func)
  {
    constexpr auto always_true = [](const Expression *) { return true; };
    this->traverse(func, always_true);
  }

  // 带条件的 后序遍历 dfs
  virtual void traverse(const std::function<void(Expression*)>& func, const std::function<bool(Expression*)>& filter)
  {
    if (filter(this)) {
      func(this);
    }
  }

  // 后序遍历 检查
  virtual RC traverse_check(const std::function<RC(Expression*)>& check_func)
  {
    return check_func(this);
  }

  virtual std::unique_ptr<Expression> deep_copy() const = 0;

  /**
   * @brief 表达式的名字，比如是字段名称，或者用户在执行SQL语句时输入的内容
   */
  virtual std::string name() const { return name_; }
  virtual void set_name(std::string name) { name_ = name; }

  /**
   * @brief 表达式的别名
   */
  virtual std::string alias() const { return alias_; }
  virtual void set_alias(std::string alias) { alias_ = alias; }

private:
  std::string name_{};
  std::string alias_{};
};

/**
 * @brief 字段表达式
 * @ingroup Expression
 */
class FieldExpr : public Expression 
{
public:
  FieldExpr() = default;
  FieldExpr(const std::string& table_name, const std::string& field_name) : table_name_(table_name), field_name_(field_name) {}
  FieldExpr(const Table *table, const FieldMeta *field) : field_(table, field), table_name_(table->name()), field_name_(field->name()) {}
  FieldExpr(const Field &field) : field_(field), table_name_(field.table_name()), field_name_(field.field_name()) {}

  virtual ~FieldExpr() = default;

  ExprType type() const override { return ExprType::FIELD; }
  AttrType value_type() const override { return field_.attr_type(); }

  Field &field() { return field_; }

  const Field &field() const { return field_; }

  const char *table_name() const { return field_.table_name(); }

  const char *field_name() const { return field_.field_name(); }

  const std::string & get_table_name() const { return table_name_; }
  const std::string & get_field_name() const { return field_name_; }
  
  RC get_value(const Tuple &tuple, Value &value) const override;

  FieldMeta get_field_meta() const { return *field_.meta(); }

  RC check_field(const std::unordered_map<std::string, Table *> &table_map,
    const std::vector<Table *> &tables, Table* default_table = nullptr,
    const std::unordered_map<std::string, std::string> & table_alias_map = {});

  std::unique_ptr<Expression> deep_copy() const override
  {
    return std::unique_ptr<FieldExpr>(new FieldExpr(*this));
  }

private:
  Field field_;
  const std::string table_name_;
  const std::string field_name_;

  int index_ = -1;
  bool is_first_ = true;
};

/**
 * @brief 常量值表达式
 * @ingroup Expression
 */
class ValueExpr : public Expression 
{
public:
  ValueExpr() = default;
  explicit ValueExpr(const Value &value) : value_(value)
  {}

  virtual ~ValueExpr() = default;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC try_get_value(Value &value) const override { value = value_; return RC::SUCCESS; }

  ExprType type() const override { return ExprType::VALUE; }

  AttrType value_type() const override { return value_.attr_type(); }

  void get_value(Value &value) const { value = value_; }

  void set_value(Value &value) {value_ = value;}
  const Value &get_value() const { return value_; }

  bool get_neg(Value &value)
  {
    switch (value_.attr_type()) {
      case INTS:{
          value.set_int(-1 * value_.get_int());
          return true;
      }break;
      case FLOATS:{
          value.set_float(-1 * value_.get_float());
          return true;
      }break;
      case DOUBLES:{
          value.set_double(-1 * value_.get_double());
          return true;
      }break;
      default:
        break;
    }
    return false;
  }

  std::unique_ptr<Expression> deep_copy() const override
  {
    return std::unique_ptr<ValueExpr>(new ValueExpr(*this));
  }

private:
  Value value_;
};

/**
 * @brief 类型转换表达式
 * @ingroup Expression
 */
class CastExpr : public Expression 
{
public:
  CastExpr(std::unique_ptr<Expression> child, AttrType cast_type);
  virtual ~CastExpr();

  ExprType type() const override
  {
    return ExprType::CAST;
  }
  RC get_value(const Tuple &tuple, Value &value) const override;

  RC try_get_value(Value &value) const override;

  AttrType value_type() const override { return cast_type_; }

  std::unique_ptr<Expression> &child() { return child_; }

  void traverse(const std::function<void(Expression*)>& func, const std::function<bool(Expression*)>& filter) override
  {
    if (filter(this)) {
      child_->traverse(func, filter);
      func(this);
    }
  }

  RC traverse_check(const std::function<RC(Expression*)>& check_func) override
  {
    if (RC rc = child_->traverse_check(check_func); RC::SUCCESS != rc) {
      return rc;
    } else if (rc = check_func(this); RC::SUCCESS != rc) {
      return rc;
    }
    return RC::SUCCESS;
  }

  std::unique_ptr<Expression> deep_copy() const override
  {
    auto new_expr = std::make_unique<CastExpr>(child_->deep_copy(), cast_type_);
    new_expr->set_name(name());
    return new_expr;
  }

private:
  RC cast(const Value &value, Value &cast_value) const;

private:
  std::unique_ptr<Expression> child_;  ///< 从这个表达式转换
  AttrType cast_type_;  ///< 想要转换成这个类型
};

/**
 * @brief 比较表达式
 * @ingroup Expression
 */
class ComparisonExpr : public Expression 
{
public:
  ComparisonExpr(CompOp comp, Expression* left, Expression* right);
  ComparisonExpr(CompOp comp, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  virtual ~ComparisonExpr();

  ExprType type() const override { return ExprType::COMPARISON; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  AttrType value_type() const override { return BOOLEANS; }

  CompOp comp() const { return comp_; }

  std::unique_ptr<Expression> &left()  { return left_;  }
  std::unique_ptr<Expression> &right() { return right_; }

  /**
   * 尝试在没有tuple的情况下获取当前表达式的值
   * 在优化的时候，可能会使用到
   */
  RC try_get_value(Value &value) const override;

  /**
   * compare the two tuple cells
   * @param value the result of comparison
   */
  RC compare_value(const Value &left, const Value &right, bool &value) const;

  bool has_rhs() const
  {
    // return right_;
    // 虽然 IS_[NOT]_NULL 的情况下 rhs 是 null ValueExpr 但仍然提供这个接口
    return comp_ != IS_NULL && comp_ != IS_NOT_NULL;
  }

  void traverse(const std::function<void(Expression*)>& func, const std::function<bool(Expression*)>& filter) override
  {
    if (filter(this)) {
      left_->traverse(func, filter);
      if (has_rhs()) {
        right_->traverse(func, filter);
      }
      func(this);
    }
  }

  RC traverse_check(const std::function<RC(Expression*)>& check_func) override
  {
    RC rc = RC::SUCCESS;
    if (RC::SUCCESS != (rc = left_->traverse_check(check_func))) {
      return rc;
    } else if (has_rhs() && RC::SUCCESS != (rc = right_->traverse_check(check_func))) {
      return rc;
    } else if (RC::SUCCESS != (rc = check_func(this))) {
      return rc;
    }
    return RC::SUCCESS;
  }

  std::unique_ptr<Expression> deep_copy() const override
  {
    std::unique_ptr<Expression> new_left = left_ ->deep_copy();
    std::unique_ptr<Expression> new_right;
    if (right_) { // NOTE: not has_rhs
      new_right = right_->deep_copy();
    }
    auto new_expr = std::make_unique<ComparisonExpr>(comp_, std::move(new_left), std::move(new_right));
    new_expr->set_name(name());
    return new_expr;
  }

private:
  CompOp comp_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
};

/**
 * @brief 联结表达式
 * @ingroup Expression
 * 多个表达式使用同一种关系(AND或OR)来联结
 * 当前miniob仅有AND操作
 */
class ConjunctionExpr : public Expression 
{
public:
  enum class Type {
    AND,
    OR,
  };

public:
  ConjunctionExpr(Type type, Expression* left, Expression* right);
  ConjunctionExpr(Type type, std::vector<std::unique_ptr<Expression>> children);
  virtual ~ConjunctionExpr() = default;

  ExprType type() const override { return ExprType::CONJUNCTION; }

  AttrType value_type() const override { return BOOLEANS; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  Type conjunction_type() const { return conjunction_type_; }

  std::vector<std::unique_ptr<Expression>> &children() { return children_; }

  void traverse(const std::function<void(Expression*)>& func, const std::function<bool(Expression*)>& filter) override
  {
    if (filter(this)) {
      for (auto& child : children_) {
        child->traverse(func, filter);
      }
      func(this);
    }
  }

  RC traverse_check(const std::function<RC(Expression*)>& check_func) override
  {
    RC rc = RC::SUCCESS;
    for (auto& child : children_) {
      if (RC::SUCCESS != (rc = child->traverse_check(check_func))) {
        return rc;
      }
    }
    if (RC::SUCCESS != (rc = check_func(this))) {
      return rc;
    }
    return RC::SUCCESS;
  }

  std::unique_ptr<Expression> deep_copy() const override
  {
    std::vector<std::unique_ptr<Expression>> new_children;
    for (auto& child : children_) {
      new_children.emplace_back(child->deep_copy());
    }
    auto new_expr = std::make_unique<ConjunctionExpr>(conjunction_type_, std::move(new_children));
    new_expr->set_name(name());
    return new_expr;
  }

private:
  Type conjunction_type_;
  std::vector<std::unique_ptr<Expression>> children_;
};

/**
 * @brief 算术表达式
 * @ingroup Expression
 */
class ArithmeticExpr : public Expression 
{
public:
  enum class Type {
    ADD,
    SUB,
    MUL,
    DIV,
    NEGATIVE,
  };

public:
  ArithmeticExpr(Type type, Expression *left, Expression *right);
  ArithmeticExpr(Type type, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  virtual ~ArithmeticExpr() = default;

  ExprType type() const override { return ExprType::ARITHMETIC; }

  AttrType value_type() const override;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC try_get_value(Value &value) const override;

  Type arithmetic_type() const { return arithmetic_type_; }

  std::unique_ptr<Expression> &left() { return left_; }
  std::unique_ptr<Expression> &right() { return right_; }

  bool has_rhs() const
  {
    // return arithmetic_type_ != Type::NEGATIVE; // logical
    return right_ != nullptr; // physical
  }

  void traverse(const std::function<void(Expression*)>& func, const std::function<bool(Expression*)>& filter) override
  {
    if (filter(this)) {
      left_->traverse(func, filter);
      if (has_rhs()) {
        right_->traverse(func, filter);
      }
      func(this);
    }
  }

  RC traverse_check(const std::function<RC(Expression*)>& check_func) override
  {
    RC rc = RC::SUCCESS;
    if (RC::SUCCESS != (rc = left_->traverse_check(check_func))) {
      return rc;
    } else if (has_rhs() && RC::SUCCESS != (rc = right_->traverse_check(check_func))) {
      return rc;
    } else if (RC::SUCCESS != (rc = check_func(this))) {
      return rc;
    }
    return RC::SUCCESS;
  }

  std::unique_ptr<Expression> deep_copy() const override
  {
    std::unique_ptr<Expression> new_left = left_ ->deep_copy();
    std::unique_ptr<Expression> new_right;
    if (right_) { // NOTE: not has_rhs
      new_right = right_->deep_copy();
    }
    auto new_expr = std::make_unique<ArithmeticExpr>(arithmetic_type_, std::move(new_left), std::move(new_right));
    new_expr->set_name(name());
    return new_expr;
  }

private:
  RC calc_value(const Value &left_value, const Value &right_value, Value &value) const;
  
private:
  Type arithmetic_type_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
};


static bool exp2value(Expression * exp,Value & value)
{
  if(exp->type() == ExprType::VALUE) {
    ValueExpr *tmp = static_cast<ValueExpr*>(exp);
    value = tmp->get_value();
    return true;
  }
  if(exp->type() == ExprType::ARITHMETIC) {
    ArithmeticExpr * tmp = static_cast<ArithmeticExpr *>(exp);
    if(tmp->arithmetic_type() != ArithmeticExpr::Type::NEGATIVE && tmp->left()->type() != ExprType::VALUE) {
      return false;
    }
    ValueExpr *lhs = static_cast<ValueExpr*>(tmp->left().get());
    if( ! lhs->get_neg(value) ) {
      LOG_WARN("get_neg error!");
      return false;
    }
    return true;
  }
  return false;
}

class AggrFuncExpr : public Expression {
public:
  AggrFuncExpr(AggrFuncType type, Expression *param);
  AggrFuncExpr(AggrFuncType type, std::unique_ptr<Expression> param);
  virtual ~AggrFuncExpr() = default;

  ExprType type() const override
  {
    return ExprType::AGGRFUNCTION;
  }
  // void set_param_constexpr(bool flag)
  // {
  //   param_is_constexpr_ = flag;
  // }
  std::unique_ptr<Expression> &get_param()
  {
    return param_;
  }
  const std::unique_ptr<Expression> &get_param() const
  {
    return param_;
  }
  RC get_value(const Tuple &tuple, Value &value) const override;

  std::string get_func_name() const;

  AttrType value_type() const override;
  AggrFuncType get_aggr_func_type() const
  {
    return type_;
  }

  // count(*) count(1) count(1+1) 需要特殊处理 null
  bool is_count_constexpr() const {
    if (type_ == AggrFuncType::AGG_COUNT && param_is_constexpr_) {
      return true;
    }
    return false;
  }

  void set_aggr_fun_type(AggrFuncType type)
  {
    type_ = type;
  }

  // 聚集函数表达式的 traverse[_check] 需要特殊对待 param 可能是个 *
  void traverse(const std::function<void(Expression*)>& func, const std::function<bool(Expression*)>& filter) override
  {
    if (filter(this)) {
      param_->traverse(func, filter);
      func(this);
    }
  }

  RC traverse_check(const std::function<RC(Expression*)>& check_func) override
  {
    RC rc = RC::SUCCESS;
    if (RC::SUCCESS != (rc = param_->traverse_check(check_func))) {
      return rc;
    } if (RC::SUCCESS != (rc = check_func(this))) {
      return rc;
    }
    return rc;
  }

  std::unique_ptr<Expression> deep_copy() const override
  {
    std::unique_ptr<Expression> new_param;
    if (param_) {
      new_param = param_->deep_copy();
    }
    auto new_expr = std::make_unique<AggrFuncExpr>(type_, std::move(new_param));
    new_expr->set_name(name());
    return new_expr;
  }

private:
  AggrFuncType type_;
  std::unique_ptr<Expression> param_;
  bool param_is_constexpr_ = false;

  bool is_first_ = true;
  int index_ = -1;
};

class SysFuncExpr : public Expression {
public:
  SysFuncExpr() = default;
  SysFuncExpr(SysFuncType func_type, std::vector<Expression*>& params) : func_type_(func_type)
  {
    for (auto expr: params) {
      params_.emplace_back(expr);
    }
  }
  SysFuncExpr(SysFuncType func_type, std::vector<std::unique_ptr<Expression>> params) : func_type_(func_type), params_(std::move(params)) {}
  virtual ~SysFuncExpr() = default;

  AttrType value_type() const override
  {
    switch (func_type_) {
      case SYS_FUNC_LENGTH:
        return INTS;
        break;
      case SYS_FUNC_ROUND:
        return FLOATS;
        break;
      case SYS_FUNC_DATE_FORMAT:
        return CHARS;
        break;
      default:
        break;
    }
    return UNDEFINED;
  }

  ExprType type() const override
  {
    return ExprType::SYSFUNCTION;
  }

  RC get_func_length_value(const Tuple &tuple, Value &value) const;

  RC get_func_round_value(const Tuple &tuple, Value &value) const;

  RC get_func_data_format_value(const Tuple &tuple, Value &value) const;

  RC get_value(const Tuple &tuple, Value &value) const override
  {
    RC rc = RC::SUCCESS;
    switch (func_type_) {
      case SYS_FUNC_LENGTH: {
        rc = get_func_length_value(tuple, value);
        break;
      }
      case SYS_FUNC_ROUND: {
        rc = get_func_round_value(tuple, value);
        break;
      }
      case SYS_FUNC_DATE_FORMAT: {
        rc = get_func_data_format_value(tuple, value);
        break;
      }
      default:
        break;
    }
    return rc;
  }

  void traverse(const std::function<void(Expression*)>& func, const std::function<bool(Expression*)>& filter) override
  {
    if (filter(this)) {
      for (auto& param : params_){
          param->traverse(func, filter);
      }
      func(this);
    }
  }

  RC traverse_check(const std::function<RC(Expression*)>& check_func) override
  {
    RC rc = RC::SUCCESS;
    for (auto& param : params_) {
      if (RC::SUCCESS != (rc = param->traverse_check(check_func))) {
        return rc;
      }
    }
    if (RC::SUCCESS != (rc = check_func(this))) {
      return rc;
    }
    return RC::SUCCESS;
  }

  std::unique_ptr<Expression> deep_copy() const override
  {
    std::vector<std::unique_ptr<Expression>> new_params;
    for (auto& param : params_) {
      new_params.emplace_back(param->deep_copy());
    }
    auto new_expr = std::make_unique<SysFuncExpr>(func_type_, std::move(new_params));
    new_expr->set_name(name());
    return new_expr;
  }

  RC check_param_type_and_number() const;

private:
  SysFuncType func_type_;
  std::vector<std::unique_ptr<Expression>> params_;
};

class SelectStmt;
class LogicalOperator;
class PhysicalOperator;
class SubQueryExpr : public Expression
{
public:
  SubQueryExpr(const SelectSqlNode& sql_node);
  virtual ~SubQueryExpr();

  RC open(Trx* trx);
  RC close();
  bool has_more_row(const Tuple &tuple) const;

  RC get_value(const Tuple &tuple, Value &value) const;

  RC try_get_value(Value &value) const;

  ExprType type() const;

  AttrType value_type() const;

  std::unique_ptr<Expression> deep_copy() const;

  RC generate_select_stmt(Db* db, const std::unordered_map<std::string, Table *> &tables);
  RC generate_logical_oper();
  RC generate_physical_oper();

private:
  std::unique_ptr<SelectSqlNode> sql_node_;
  std::unique_ptr<SelectStmt> stmt_;
  std::unique_ptr<LogicalOperator> logical_oper_;
  std::unique_ptr<PhysicalOperator> physical_oper_;
};

class ExprListExpr : public Expression
{
public:
  ExprListExpr(std::vector<Expression*>&& exprs) {
    for (auto expr : exprs) {
      exprs_.emplace_back(expr);
    }
    exprs.clear();
  }
  ExprListExpr(std::vector<std::unique_ptr<Expression>>&& exprs) : exprs_(std::move(exprs)) {}
  virtual ~ExprListExpr() = default;

  void reset()
  {
    cur_idx_ = 0;
  }

  RC get_value(const Tuple &tuple, Value &value) const override
  {
    if (cur_idx_ >= exprs_.size()) {
      return RC::RECORD_EOF;
    }
    return exprs_[const_cast<int&>(cur_idx_)++]->get_value(tuple, value);
  }

  RC try_get_value(Value &value) const override { return RC::UNIMPLENMENT; }

  ExprType type() const override { return ExprType::EXPRLIST; }

  AttrType value_type() const override { return UNDEFINED; }

  void traverse(const std::function<void(Expression*)>& func, const std::function<bool(Expression*)>& filter) override
  {
    if (filter(this)) {
      for (auto& expr : exprs_) {
        expr->traverse(func, filter);
      }
      func(this);
    }
  }

  RC traverse_check(const std::function<RC(Expression*)>& check_func) override
  {
    RC rc = RC::SUCCESS;
    for (auto& expr : exprs_) {
      if (RC::SUCCESS != (rc = expr->traverse_check(check_func))) {
        return rc;
      }
    }
    if (RC::SUCCESS != (rc = check_func(this))) {
      return rc;
    }
    return RC::SUCCESS;
  }

  std::unique_ptr<Expression> deep_copy() const override
  {
    std::vector<std::unique_ptr<Expression>> new_children;
    for (auto& expr : exprs_) {
      new_children.emplace_back(expr->deep_copy());
    }
    auto new_expr = std::make_unique<ExprListExpr>(std::move(new_children));
    new_expr->set_name(name());
    new_expr->set_alias(alias());
    return new_expr;
  }
private:
  int cur_idx_ = 0;
  std::vector<std::unique_ptr<Expression>> exprs_;
};