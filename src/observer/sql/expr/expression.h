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

  /**
   * @brief 检查 FieldExpr
   * @details 在生成 SelectStmt 的时候调用
   */
  virtual RC check_field(const std::unordered_map<std::string, Table *> &table_map,
    const std::vector<Table *> &tables, Db *db, Table* default_table = nullptr) {
      return RC::SUCCESS;
    }

  /**
   * @brief 检查是否能下推
   * @details 在生成 SelectStmt 的时候调用，如果当前表达式树中存在一个 FieldExpr 的 table name 不在 table_map 中就返回 false
   */
  virtual bool check_can_push_down(const std::unordered_map<std::string, Table *> &table_map) {
    return true;
  }

  /**
   * @brief 表达式的名字，比如是字段名称，或者用户在执行SQL语句时输入的内容
   */
  virtual std::string name() const { return name_; }
  virtual void set_name(std::string name) { name_ = name; }

private:
  std::string  name_;
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

  RC check_field(const std::unordered_map<std::string, Table *> &table_map,
    const std::vector<Table *> &tables, Db *db, Table* default_table = nullptr) override;

  bool check_can_push_down(const std::unordered_map<std::string, Table *> &table_map) override {
    return table_map.count(table_name_) != 0;
  }
private:
  Field field_;
  const std::string table_name_;
  const std::string field_name_;
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
    switch (value_.attr_type())
    {
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

  RC check_field(const std::unordered_map<std::string, Table *> &table_map,
    const std::vector<Table *> &tables, Db *db, Table* default_table = nullptr) override {
      return child_->check_field(table_map, tables, db, default_table);
    }
  bool check_can_push_down(const std::unordered_map<std::string, Table *> &table_map) override {
    return child_->check_can_push_down(table_map);
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

  virtual RC check_field(const std::unordered_map<std::string, Table *> &table_map,
    const std::vector<Table *> &tables, Db *db, Table* default_table = nullptr) override {
      if (RC rc = left_->check_field(table_map, tables, db, default_table); rc != RC::SUCCESS) {
        return rc;
      } else if (rc = right_->check_field(table_map, tables, db, default_table); rc != RC::SUCCESS) {
        return rc;
      }
      return RC::SUCCESS;
    }

  bool check_can_push_down(const std::unordered_map<std::string, Table *> &table_map) override {
    return left_->check_can_push_down(table_map) && right_->check_can_push_down(table_map);
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
  ConjunctionExpr(Type type, std::vector<std::unique_ptr<Expression>> children);
  virtual ~ConjunctionExpr() = default;

  ExprType type() const override { return ExprType::CONJUNCTION; }

  AttrType value_type() const override { return BOOLEANS; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  Type conjunction_type() const { return conjunction_type_; }

  std::vector<std::unique_ptr<Expression>> &children() { return children_; }

  virtual RC check_field(const std::unordered_map<std::string, Table *> &table_map,
    const std::vector<Table *> &tables, Db *db, Table* default_table = nullptr) override {
      for (auto& expr : children_) {
        if (RC rc = expr->check_field(table_map, tables, db, default_table); rc != RC::SUCCESS) {
          return rc;
        }
      }
      return RC::SUCCESS;
    }

  bool check_can_push_down(const std::unordered_map<std::string, Table *> &table_map) override {
    for (auto& expr : children_) {
      if (!expr->check_can_push_down(table_map)) {
        return false;
      }
    }
    return true;
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

  virtual RC check_field(const std::unordered_map<std::string, Table *> &table_map,
    const std::vector<Table *> &tables, Db *db, Table* default_table = nullptr) override {
      RC rc = RC::SUCCESS;
      if (rc = left_->check_field(table_map, tables, db, default_table); rc != RC::SUCCESS) {
        return rc;
      }
      if (arithmetic_type_ != Type::NEGATIVE) {
        ASSERT(right_, "ERROR!");
        if (rc = right_->check_field(table_map, tables, db, default_table); rc != RC::SUCCESS) {
          return rc;
        }
      }
      return RC::SUCCESS;
    }

  bool check_can_push_down(const std::unordered_map<std::string, Table *> &table_map) override {
    if (!left_->check_can_push_down(table_map)) {
      return false;
    }
    if (arithmetic_type_ != Type::NEGATIVE) {
      ASSERT(right_, "ERROR!");
      return right_->check_can_push_down(table_map);
    }
    return true;
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