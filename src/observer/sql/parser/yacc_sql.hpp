/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_YACC_SQL_HPP_INCLUDED
# define YY_YY_YACC_SQL_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    SEMICOLON = 258,
    CREATE = 259,
    DROP = 260,
    TABLE = 261,
    TABLES = 262,
    INDEX = 263,
    VIEW = 264,
    CALC = 265,
    SELECT = 266,
    DESC = 267,
    SHOW = 268,
    SYNC = 269,
    INSERT = 270,
    DELETE = 271,
    UPDATE = 272,
    LBRACE = 273,
    RBRACE = 274,
    COMMA = 275,
    TRX_BEGIN = 276,
    TRX_COMMIT = 277,
    TRX_ROLLBACK = 278,
    INT_T = 279,
    STRING_T = 280,
    FLOAT_T = 281,
    DATE_T = 282,
    TEXT_T = 283,
    HELP = 284,
    EXIT = 285,
    DOT = 286,
    INTO = 287,
    VALUES = 288,
    FROM = 289,
    WHERE = 290,
    AND = 291,
    OR = 292,
    SET = 293,
    ON = 294,
    LOAD_DATA = 295,
    INFILE = 296,
    EXPLAIN = 297,
    IS = 298,
    NULL_T = 299,
    INNER = 300,
    JOIN = 301,
    AS = 302,
    IN = 303,
    EXISTS = 304,
    EQ = 305,
    LT = 306,
    GT = 307,
    LE = 308,
    GE = 309,
    NE = 310,
    NOT = 311,
    LIKE = 312,
    UNIQUE = 313,
    AGGR_MAX = 314,
    AGGR_MIN = 315,
    AGGR_SUM = 316,
    AGGR_AVG = 317,
    AGGR_COUNT = 318,
    LENGTH = 319,
    ROUND = 320,
    DATE_FORMAT = 321,
    ORDER = 322,
    GROUP = 323,
    BY = 324,
    ASC = 325,
    HAVING = 326,
    NUMBER = 327,
    FLOAT = 328,
    ID = 329,
    SSS = 330,
    DATE_STR = 331,
    UMINUS = 332
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 152 "yacc_sql.y"

  ParsedSqlNode *                   sql_node;
  Value *                           value;
  enum CompOp                       comp;
  RelAttrSqlNode *                  rel_attr;
  std::vector<AttrInfoSqlNode> *    attr_infos;
  AttrInfoSqlNode *                 attr_info;
  Expression *                      expression;
  UpdateKV *                        update_kv;
  std::vector<UpdateKV> *           update_kv_list;
  std::vector<Expression *> *       expression_list;
  std::vector<Value> *              value_list;
  std::vector<std::string> *        relation_list;
  std::vector<std::vector<Value>> * insert_value_list;
  std::vector<RelAttrSqlNode> *     rel_attr_list;
  InnerJoinSqlNode *                inner_joins;
  std::vector<InnerJoinSqlNode> *   inner_joins_list;
  OrderBySqlNode*                   orderby_unit;
  std::vector<OrderBySqlNode> *     orderby_unit_list;
  char *                            string;
  int                               number;
  float                             floats;
  bool                              boolean;

#line 160 "yacc_sql.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner);

#endif /* !YY_YY_YACC_SQL_HPP_INCLUDED  */
