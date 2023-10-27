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
// Created by Meiyi
//

#pragma once

#include <stddef.h>
#include <memory>
#include <vector>
#include <string>

#include "sql/parser/value.h"

class Expression;
class FieldExpr;
class OrderByUnit;

typedef enum { AGG_MAX, AGG_MIN, AGG_SUM, AGG_AVG, AGG_COUNT, AGGR_FUNC_TYPE_NUM } AggrFuncType;
typedef enum { SYS_FUNC_LENGTH, SYS_FUNC_ROUND, SYS_FUNC_DATE_FORMAT,SYS_FUNC_TYPE_NUM } SysFuncType;
/**
 * @defgroup SQLParser SQL Parser 
 */

/**
 * @brief 描述一个属性
 * @ingroup SQLParser
 * @details 属性，或者说字段(column, field)
 * Rel -> Relation
 * Attr -> Attribute
 */
struct RelAttrSqlNode
{
  std::string relation_name;   ///< relation name (may be NULL) 表名
  std::string attribute_name;  ///< attribute name              属性名
};

/**
 * @brief 描述比较运算符
 * @ingroup SQLParser
 */
enum CompOp 
{
  EQUAL_TO,     ///< "="
  LESS_EQUAL,   ///< "<="
  NOT_EQUAL,    ///< "<>"
  LESS_THAN,    ///< "<"
  GREAT_EQUAL,  ///< ">="
  GREAT_THAN,   ///< ">"
  LIKE_OP,      ///< "like"
  NOT_LIKE_OP,  ///< "not like"
  IS_NULL,      ///< is null
  IS_NOT_NULL,  ///< is not null
  // -------
  IN_OP,        ///< in (sub query)
  NOT_IN_OP,    ///< not in (sub query)
  EXISTS_OP,    ///< exists (sub query)
  NOT_EXISTS_OP,///< not exists (sub query)
  // ------- 上面这四个连着不要拆开
  NO_OP
};


struct OrderBySqlNode
{
  Expression * expr = nullptr;
  bool is_asc;// true 为升序
};

/**
 * @brief 描述一串 inner join
 * @ingroup SQLParser
 * @details t1 inner join t2 on condition
 */
struct InnerJoinSqlNode
{
  std::pair<std::string, std::string> base_relation;
  std::vector<std::pair<std::string, std::string>> join_relations;
  std::vector<Expression*> conditions;
};

/**
 * @brief 描述一个select语句
 * @ingroup SQLParser
 * @details 一个正常的select语句描述起来比这个要复杂很多，这里做了简化。
 * 一个select语句由三部分组成，分别是select, from, where。
 * select部分表示要查询的字段，from部分表示要查询的表，where部分表示查询的条件。
 */

struct SelectSqlNode
{
  std::vector<Expression *>       project_exprs; ///< attributes in select clause
  std::vector<InnerJoinSqlNode>   relations;///< 查询的表
  Expression *       conditions = nullptr;    ///< 查询条件
  std::vector<OrderBySqlNode>     orderbys; ///< attributes in order clause
  std::vector<Expression *>       groupby_exprs;  ///< groupby 
  Expression *       having_conditions = nullptr;  ///< groupby having
};

/**
 * @brief 算术表达式计算的语法树
 * @ingroup SQLParser
 */
struct CalcSqlNode
{
  std::vector<Expression *> expressions;  ///< calc clause

  ~CalcSqlNode();
};

/**
 * @brief 描述一个insert语句
 * @ingroup SQLParser
 * @details 于Selects类似，也做了很多简化
 */
struct InsertSqlNode
{
  std::string        relation_name;  ///< Relation to insert into
  std::vector<std::string> attrs_name;    ///< 要插入的目标列
  std::vector<std::vector<Value>> values;         ///< 要插入的值
};

/**
 * @brief 描述一个delete语句
 * @ingroup SQLParser
 */
struct DeleteSqlNode
{
  std::string    relation_name;  ///< Relation to delete from
  Expression*    conditions = nullptr;
};

/**
 * @brief 描述一个update语句
 * @ingroup SQLParser
 */
struct UpdateKV
{
  std::string attr_name;
  Expression* value = nullptr;
};
struct UpdateSqlNode
{
  std::string                   relation_name;         ///< Relation to update
  std::vector<std::string>      attribute_names;       ///< 更新的字段，仅支持多个字段
  std::vector<Expression*>      values;                ///< 更新的值，仅支持多个字段
  Expression*                   conditions = nullptr;
};

/**
 * @brief 描述一个属性
 * @ingroup SQLParser
 * @details 属性，或者说字段(column, field)
 * Rel -> Relation
 * Attr -> Attribute
 */
struct AttrInfoSqlNode
{
  AttrType    type;       ///< Type of attribute
  std::string name;       ///< Attribute name
  size_t      length;     ///< Length of attribute
  bool        nullable;   ///< Nullable
};

/**
 * @brief 描述一个create table语句
 * @ingroup SQLParser
 * @details 这里也做了很多简化。
 */
struct CreateTableSqlNode
{
  std::string                  relation_name;         ///< Relation name
  std::vector<AttrInfoSqlNode> attr_infos;            ///< attributes
};

/**
 * @brief 描述一个drop table语句
 * @ingroup SQLParser
 */
struct DropTableSqlNode
{
  std::string relation_name;  ///< 要删除的表名
};

/**
 * @brief 描述一个create index语句
 * @ingroup SQLParser
 * @details 创建索引时，需要指定索引名，表名，字段名。
 * 正常的SQL语句中，一个索引可能包含了多个字段，这里仅支持一个字段。
 */
struct CreateIndexSqlNode
{
  bool unique;                            ///< Unique Index
  std::string index_name;                 ///< Index name
  std::string relation_name;              ///< Relation name
  std::vector<std::string> attr_names;    ///< Attribute names
};

/**
 * @brief 描述一个drop index语句
 * @ingroup SQLParser
 */
struct DropIndexSqlNode
{
  std::string index_name;     ///< Index name
  std::string relation_name;  ///< Relation name
};

/**
 * @brief 描述一个show index语句
 * @ingroup SQLParser
 */
struct ShowIndexSqlNode
{
  std::string relation_name;  ///< Relation name
};

/**
 * @brief 描述一个desc table语句
 * @ingroup SQLParser
 * @details desc table 是查询表结构信息的语句
 */
struct DescTableSqlNode
{
  std::string relation_name;
};

/**
 * @brief 描述一个load data语句
 * @ingroup SQLParser
 * @details 从文件导入数据到表中。文件中的每一行就是一条数据，每行的数据类型、字段个数都与表保持一致
 */
struct LoadDataSqlNode
{
  std::string relation_name;
  std::string file_name;
};

/**
 * @brief 设置变量的值
 * @ingroup SQLParser
 * @note 当前还没有查询变量
 */
struct SetVariableSqlNode
{
  std::string name;
  Value       value;
};

class ParsedSqlNode;

/**
 * @brief 描述一个explain语句
 * @ingroup SQLParser
 * @details 会创建operator的语句，才能用explain输出执行计划。
 * 一个command就是一个语句，比如select语句，insert语句等。
 * 可能改成SqlCommand更合适。
 */
struct ExplainSqlNode
{
  std::unique_ptr<ParsedSqlNode> sql_node;
};

/**
 * @brief 解析SQL语句出现了错误
 * @ingroup SQLParser
 * @details 当前解析时并没有处理错误的行号和列号
 */
struct ErrorSqlNode
{
  std::string error_msg;
  int         line;
  int         column;
  bool        flag;
};

/**
 * @brief 表示一个SQL语句的类型
 * @ingroup SQLParser
 */
enum SqlCommandFlag
{
  SCF_ERROR = 0,
  SCF_CALC,
  SCF_SELECT,
  SCF_INSERT,
  SCF_UPDATE,
  SCF_DELETE,
  SCF_CREATE_TABLE,
  SCF_DROP_TABLE,
  SCF_CREATE_INDEX,
  SCF_DROP_INDEX,
  SCF_SYNC,
  SCF_SHOW_INDEX,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_BEGIN,        ///< 事务开始语句，可以在这里扩展只读事务
  SCF_COMMIT,
  SCF_CLOG_SYNC,
  SCF_ROLLBACK,
  SCF_LOAD_DATA,
  SCF_HELP,
  SCF_EXIT,
  SCF_EXPLAIN,
  SCF_SET_VARIABLE, ///< 设置变量
};
/**
 * @brief 表示一个SQL语句
 * @ingroup SQLParser
 */
class ParsedSqlNode
{
public:
  enum SqlCommandFlag       flag;
  ErrorSqlNode              error;
  CalcSqlNode               calc;
  SelectSqlNode             selection;
  InsertSqlNode             insertion;
  DeleteSqlNode             deletion;
  UpdateSqlNode             update;
  CreateTableSqlNode        create_table;
  DropTableSqlNode          drop_table;
  CreateIndexSqlNode        create_index;
  DropIndexSqlNode          drop_index;
  ShowIndexSqlNode          show_index;
  DescTableSqlNode          desc_table;
  LoadDataSqlNode           load_data;
  ExplainSqlNode            explain;
  SetVariableSqlNode        set_variable;

public:
  ParsedSqlNode();
  explicit ParsedSqlNode(SqlCommandFlag flag);
};

/**
 * @brief 表示语法解析后的数据
 * @ingroup SQLParser
 */
class ParsedSqlResult
{
public:
  void add_sql_node(std::unique_ptr<ParsedSqlNode> sql_node);
  std::vector<std::unique_ptr<ParsedSqlNode>> &sql_nodes()
  {
    return sql_nodes_;
  }

private:
  std::vector<std::unique_ptr<ParsedSqlNode>> sql_nodes_;  ///< 这里记录SQL命令。虽然看起来支持多个，但是当前仅处理一个
};
