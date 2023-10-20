/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "common/log/log.h"
#include "common/lang/string.h"
#include "common/time/date.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.hpp"
#include "sql/parser/lex_sql.h"
#include "sql/expr/expression.h"

using namespace std;

string token_name(const char *sql_string, YYLTYPE *llocp)
{
  return string(sql_string + llocp->first_column, llocp->last_column - llocp->first_column + 1);
}

int yyerror(YYLTYPE *llocp, const char *sql_string, ParsedSqlResult *sql_result, yyscan_t scanner, const char *msg, bool flag = false)
{
  std::unique_ptr<ParsedSqlNode> error_sql_node = std::make_unique<ParsedSqlNode>(SCF_ERROR);
  error_sql_node->error.error_msg = msg;
  error_sql_node->error.line = llocp->first_line;
  error_sql_node->error.column = llocp->first_column;
  error_sql_node->error.flag = flag;
  sql_result->add_sql_node(std::move(error_sql_node));
  return 0;
}

ArithmeticExpr *create_arithmetic_expression(ArithmeticExpr::Type type,
                                             Expression *left,
                                             Expression *right,
                                             const char *sql_string,
                                             YYLTYPE *llocp)
{
  ArithmeticExpr *expr = new ArithmeticExpr(type, left, right);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}


#line 116 "yacc_sql.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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
    CALC = 264,
    SELECT = 265,
    DESC = 266,
    SHOW = 267,
    SYNC = 268,
    INSERT = 269,
    DELETE = 270,
    UPDATE = 271,
    LBRACE = 272,
    RBRACE = 273,
    COMMA = 274,
    TRX_BEGIN = 275,
    TRX_COMMIT = 276,
    TRX_ROLLBACK = 277,
    INT_T = 278,
    STRING_T = 279,
    FLOAT_T = 280,
    DATE_T = 281,
    HELP = 282,
    EXIT = 283,
    DOT = 284,
    INTO = 285,
    VALUES = 286,
    FROM = 287,
    WHERE = 288,
    AND = 289,
    SET = 290,
    ON = 291,
    LOAD = 292,
    DATA = 293,
    INFILE = 294,
    EXPLAIN = 295,
    IS = 296,
    NULL_T = 297,
    EQ = 298,
    LT = 299,
    GT = 300,
    LE = 301,
    GE = 302,
    NE = 303,
    NOT = 304,
    LIKE = 305,
    NUMBER = 306,
    FLOAT = 307,
    ID = 308,
    SSS = 309,
    DATE_STR = 310,
    UMINUS = 311
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 110 "yacc_sql.y"

  ParsedSqlNode *                   sql_node;
  ConditionSqlNode *                condition;
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
  std::vector<std::vector<Value>> * insert_value_list;
  std::vector<ConditionSqlNode> *   condition_list;
  std::vector<RelAttrSqlNode> *     rel_attr_list;
  std::vector<std::string> *        relation_list;
  char *                            string;
  int                               number;
  float                             floats;

#line 247 "yacc_sql.cpp"

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



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  68
/* YYLAST -- Last index in YYTABLE.  */
<<<<<<< HEAD
<<<<<<< HEAD
#define YYLAST   176
=======
#define YYLAST   197
>>>>>>> expression
=======
#define YYLAST   171
>>>>>>> add minus for expression

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
<<<<<<< HEAD
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  107
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  193
=======
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  98
/* YYNSTATES -- Number of states.  */
<<<<<<< HEAD
#define YYNSTATES  177
>>>>>>> expression
=======
#define YYNSTATES  176
>>>>>>> add minus for expression

#define YYUNDEFTOK  2
#define YYMAXUTOK   311


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    58,    56,     2,    57,     2,    59,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    60
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
<<<<<<< HEAD
       0,   189,   189,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   220,   226,   231,   237,   243,   249,   255,
     262,   268,   276,   290,   300,   319,   322,   335,   344,   353,
     362,   371,   380,   391,   394,   395,   396,   397,   400,   416,
     419,   432,   447,   450,   461,   465,   469,   486,   491,   498,
     510,   534,   537,   549,   558,   580,   590,   595,   606,   609,
     612,   615,   618,   622,   625,   637,   644,   656,   661,   672,
     675,   689,   692,   705,   708,   713,   718,   725,   737,   749,
     761,   774,   784,   794,   804,   817,   818,   819,   820,   821,
     822,   823,   824,   828,   841,   849,   859,   860
=======
       0,   185,   185,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   216,   222,   227,   233,   239,   245,   251,
     258,   264,   272,   286,   296,   315,   318,   331,   340,   349,
     358,   367,   376,   387,   390,   391,   392,   393,   396,   412,
<<<<<<< HEAD
     415,   428,   443,   446,   457,   461,   465,   482,   487,   494,
     506,   522,   544,   554,   559,   570,   573,   576,   579,   582,
<<<<<<< HEAD
     586,   589,   598,   609,   639,   646,   652,   657,   685,   688,
     701,   704,   709,   714,   721,   728,   739,   842,   843,   844,
     845,   846,   847,   848,   849,   853,   866,   874,   884,   885
>>>>>>> expression
=======
     586,   589,   594,   605,   635,   642,   648,   653,   681,   684,
     697,   700,   705,   710,   717,   724,   735,   838,   839,   840,
     841,   842,   843,   844,   845,   849,   862,   870,   880,   881
>>>>>>> expression support null
=======
     415,   428,   449,   452,   469,   473,   477,   494,   499,   506,
     518,   540,   562,   572,   577,   588,   591,   594,   597,   600,
     604,   607,   623,   653,   660,   666,   671,   699,   702,   715,
     718,   723,   728,   735,   742,   753,   856,   857,   858,   859,
     860,   861,   862,   863,   867,   880,   888,   898,   899
>>>>>>> add minus for expression
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SEMICOLON", "CREATE", "DROP", "TABLE",
  "TABLES", "INDEX", "CALC", "SELECT", "DESC", "SHOW", "SYNC", "INSERT",
  "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "DATE_T",
  "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM", "WHERE", "AND", "SET",
  "ON", "LOAD", "DATA", "INFILE", "EXPLAIN", "IS", "NULL_T", "EQ", "LT",
  "GT", "LE", "GE", "NE", "NOT", "LIKE", "NUMBER", "FLOAT", "ID", "SSS",
  "DATE_STR", "'+'", "'-'", "'*'", "'/'", "UMINUS", "$accept", "commands",
  "command_wrapper", "exit_stmt", "help_stmt", "sync_stmt", "begin_stmt",
  "commit_stmt", "rollback_stmt", "drop_table_stmt", "show_tables_stmt",
  "desc_table_stmt", "create_index_stmt", "drop_index_stmt",
  "create_table_stmt", "attr_def_list", "attr_def", "number", "type",
  "insert_stmt", "insert_value_list", "insert_value", "value_list",
<<<<<<< HEAD
  "value", "delete_stmt", "update_stmt", "update_kv_list", "update_kv",
  "select_stmt", "calc_stmt", "expression_list", "expression",
  "select_attr", "rel_attr", "attr_list", "rel_list", "where",
  "condition_list", "condition", "comp_op", "load_data_stmt",
=======
  "value", "delete_stmt", "update_stmt", "select_stmt", "calc_stmt",
  "expression_list", "expression", "select_attr", "rel_attr", "rel_list",
  "where", "condition_list", "condition", "comp_op", "load_data_stmt",
>>>>>>> expression
  "explain_stmt", "set_variable_stmt", "opt_semicolon", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,    43,    45,    42,    47,
     311
};
# endif

<<<<<<< HEAD
<<<<<<< HEAD
#define YYPACT_NINF (-112)
=======
#define YYPACT_NINF (-99)
>>>>>>> expression
=======
#define YYPACT_NINF (-101)
>>>>>>> add minus for expression

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
<<<<<<< HEAD
<<<<<<< HEAD
      73,    22,    44,    -6,     0,   -49,     3,  -112,   -23,     2,
      -9,  -112,  -112,  -112,  -112,  -112,     9,    -1,    73,    47,
      70,  -112,  -112,  -112,  -112,  -112,  -112,  -112,  -112,  -112,
    -112,  -112,  -112,  -112,  -112,  -112,  -112,  -112,  -112,  -112,
    -112,    21,    27,    28,    37,    -6,  -112,  -112,  -112,  -112,
    -112,    -6,  -112,  -112,    -2,    62,  -112,    43,    77,  -112,
    -112,    39,    45,    64,    54,    63,  -112,  -112,  -112,  -112,
      86,    68,  -112,    69,   -16,  -112,    -6,    -6,    -6,    -6,
      -6,    53,    56,    58,  -112,    76,    90,    79,   -28,    80,
      82,    83,    84,  -112,  -112,     1,     1,  -112,  -112,  -112,
     114,    77,   121,   -33,  -112,    96,   122,  -112,   110,    46,
     123,   126,  -112,    92,    90,  -112,   -28,   128,    71,    81,
    -112,   115,   -28,    79,    90,   142,  -112,  -112,  -112,  -112,
     -11,    82,   132,    98,   114,  -112,   133,   121,  -112,   -37,
    -112,  -112,  -112,  -112,  -112,  -112,   103,  -112,   -33,   -34,
     -33,   -33,  -112,   122,  -112,   101,   104,  -112,   116,   123,
    -112,   138,  -112,   -28,   139,   128,  -112,   117,  -112,  -112,
    -112,  -112,   118,  -112,  -112,  -112,  -112,  -112,  -112,   143,
    -112,  -112,  -112,   133,  -112,  -112,  -112,  -112,   -17,  -112,
    -112,   120,  -112
=======
     126,     0,     7,    72,    28,   -46,     3,   -99,   -12,   -13,
     -32,   -99,   -99,   -99,   -99,   -99,   -30,    -7,   126,    24,
      22,   -99,   -99,   -99,   -99,   -99,   -99,   -99,   -99,   -99,
     -99,   -99,   -99,   -99,   -99,   -99,   -99,   -99,   -99,   -99,
     -99,   -21,   -19,    -6,    -5,    72,   -99,   -99,   -99,    32,
     -99,   -99,    72,   -99,   -99,    54,   -99,   -99,   -99,    19,
     -99,   -99,     9,    15,    34,    29,    35,   -99,   -99,   -99,
     -99,    59,    48,   -99,    61,    -2,    18,   -25,    72,    72,
      72,    72,    72,   -99,    49,    46,    68,    50,   -25,    53,
      62,    63,    64,   -99,   -99,   -99,   138,   138,   -25,   -25,
      99,   102,    72,   -99,    77,   -99,    91,    41,   103,   111,
     -99,    79,    68,   -25,   114,   127,   -99,   100,   -25,   137,
     -99,   -99,   -99,   -99,   -14,    62,   131,    92,    99,   -99,
     125,   102,   -99,   -38,   -99,   -99,   -99,   -99,   -99,   -99,
     101,   -99,    72,    72,    68,    97,   104,   -99,   110,   103,
     -99,   139,   -99,   -25,   140,   114,   -99,   117,   -99,    36,
     -99,   -99,   -99,   -99,   142,   -99,   -99,   -99,   125,   -99,
     -99,   -99,   -37,   -99,   -99,   120,   -99
>>>>>>> expression
=======
      73,    18,    22,     8,   -13,   -35,    16,  -101,     3,    23,
     -26,  -101,  -101,  -101,  -101,  -101,     5,    28,    73,    74,
      70,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,    27,    37,    38,    39,     8,  -101,  -101,  -101,    47,
    -101,  -101,     8,  -101,  -101,    -5,  -101,  -101,  -101,    49,
    -101,  -101,    45,    53,    64,    68,    87,  -101,  -101,  -101,
    -101,    92,    95,  -101,    96,   -10,    80,  -101,     8,     8,
       8,     8,     8,    81,   104,   103,    84,   -39,    85,    88,
      89,    90,  -101,  -101,  -101,   -53,   -53,  -101,  -101,   119,
     123,     8,  -101,   101,  -101,   115,    -4,   127,   130,  -101,
      97,   103,     8,   129,    71,  -101,   117,     8,   143,  -101,
    -101,  -101,  -101,    15,    88,   134,   100,   119,  -101,    12,
     123,  -101,   -40,  -101,  -101,  -101,  -101,  -101,  -101,   105,
    -101,     8,     8,    46,   106,   107,  -101,   112,   127,  -101,
     138,  -101,     8,   139,   129,  -101,   118,  -101,    66,  -101,
    -101,  -101,  -101,   144,  -101,  -101,  -101,    12,  -101,  -101,
    -101,   -32,  -101,  -101,   121,  -101
>>>>>>> add minus for expression
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    25,     0,     0,
       0,    26,    27,    28,    24,    23,     0,     0,     0,     0,
<<<<<<< HEAD
<<<<<<< HEAD
     106,    22,    21,    14,    15,    16,    17,     9,    10,    11,
      12,    13,     8,     5,     7,     6,     4,     3,    18,    19,
      20,     0,     0,     0,     0,     0,    58,    54,    55,    57,
      56,     0,    74,    65,    66,    77,    75,     0,    79,    31,
      30,     0,     0,     0,     0,     0,   104,     1,   107,     2,
       0,     0,    29,     0,     0,    73,     0,     0,     0,     0,
       0,     0,     0,     0,    76,     0,    83,     0,     0,     0,
       0,     0,     0,    72,    67,    68,    69,    70,    71,    78,
      81,    79,     0,     0,    59,     0,    61,   105,     0,     0,
      35,     0,    33,     0,    83,    80,     0,    49,     0,     0,
      84,    85,     0,     0,    83,     0,    44,    45,    46,    47,
      40,     0,     0,     0,    81,    64,    52,     0,    48,     0,
      95,    96,    97,    98,    99,   100,     0,   101,     0,     0,
       0,     0,    63,    61,    60,     0,     0,    42,     0,    35,
      34,     0,    82,     0,     0,    49,    94,     0,   102,    88,
      90,    92,     0,    87,    89,    86,    62,   103,    43,     0,
      41,    36,    32,    52,    51,    50,    93,    91,    37,    53,
      39,     0,    38
=======
      98,    22,    21,    14,    15,    16,    17,     9,    10,    11,
=======
      97,    22,    21,    14,    15,    16,    17,     9,    10,    11,
>>>>>>> add minus for expression
      12,    13,     8,     5,     7,     6,     4,     3,    18,    19,
      20,     0,     0,     0,     0,     0,    58,    54,    55,    75,
      57,    56,     0,    71,    62,    63,    72,    73,    74,     0,
      31,    30,     0,     0,     0,     0,     0,    95,     1,    98,
       2,     0,     0,    29,     0,     0,     0,    70,     0,     0,
<<<<<<< HEAD
       0,     0,     0,    72,     0,     0,    80,     0,     0,     0,
       0,     0,     0,    69,    77,    64,    65,    66,    67,    68,
      78,     0,     0,    59,     0,    97,     0,     0,    35,     0,
      33,     0,    80,     0,    49,     0,    81,    82,     0,     0,
      44,    45,    46,    47,    40,     0,     0,     0,    78,    61,
      52,     0,    48,     0,    87,    88,    89,    90,    91,    92,
       0,    93,     0,     0,    80,     0,     0,    42,     0,    35,
      34,     0,    79,     0,     0,    49,    85,     0,    94,    84,
      83,    60,    95,    43,     0,    41,    36,    32,    52,    51,
      50,    86,    37,    53,    39,     0,    38
>>>>>>> expression
=======
       0,     0,     0,     0,     0,    79,     0,     0,     0,     0,
       0,     0,    69,    76,    64,    65,    66,    67,    68,    77,
       0,     0,    59,     0,    96,     0,     0,    35,     0,    33,
       0,    79,     0,    49,     0,    80,    81,     0,     0,    44,
      45,    46,    47,    40,     0,     0,     0,    77,    61,    52,
       0,    48,     0,    86,    87,    88,    89,    90,    91,     0,
      92,     0,     0,    79,     0,     0,    42,     0,    35,    34,
       0,    78,     0,     0,    49,    84,     0,    93,    83,    82,
      60,    94,    43,     0,    41,    36,    32,    52,    51,    50,
      85,    37,    53,    39,     0,    38
>>>>>>> add minus for expression
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
<<<<<<< HEAD
<<<<<<< HEAD
    -112,  -112,   145,  -112,  -112,  -112,  -112,  -112,  -112,  -112,
    -112,  -112,  -112,  -112,  -112,     5,    34,  -112,  -112,  -112,
       4,    29,   -15,   -87,  -112,  -112,    14,    48,  -112,  -112,
      94,   -12,  -112,    -4,    72,    38,  -111,    23,  -112,    57,
    -112,  -112,  -112,  -112
=======
     -99,   -99,   146,   -99,   -99,   -99,   -99,   -99,   -99,   -99,
     -99,   -99,   -99,   -99,   -99,    16,    31,   -99,   -99,   -99,
      12,    56,    20,   -55,   -99,   -99,   -99,   -99,    -3,   -43,
     -99,   -99,    66,   -98,    52,   -99,   -99,   -99,   -99,   -99,
     -99
>>>>>>> expression
=======
    -101,  -101,   146,  -101,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,  -101,  -101,  -101,  -101,    13,    41,  -101,  -101,  -101,
      14,    36,     0,    82,  -101,  -101,  -101,  -101,    -3,   -45,
    -101,  -101,    43,  -100,    29,  -101,  -101,  -101,  -101,  -101,
    -101
>>>>>>> add minus for expression
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
<<<<<<< HEAD
<<<<<<< HEAD
      28,    29,    30,    31,    32,   132,   110,   179,   130,    33,
     138,   117,   164,    52,    34,    35,   124,   106,    36,    37,
      53,    54,    57,   119,    84,   114,   104,   120,   121,   148,
      38,    39,    40,    69
=======
      28,    29,    30,    31,    32,   126,   108,   164,   124,    33,
     132,   114,   154,    53,    34,    35,    36,    37,    54,    55,
      59,    56,   112,   103,   116,   117,   142,    38,    39,    40,
=======
      28,    29,    30,    31,    32,   125,   107,   163,   123,    33,
     131,   113,   153,    53,    34,    35,    36,    37,    54,    55,
      59,    56,   111,   102,   115,   116,   141,    38,    39,    40,
>>>>>>> add minus for expression
      70
>>>>>>> expression
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
<<<<<<< HEAD
<<<<<<< HEAD
      58,   107,    93,   135,    59,   166,   156,    61,   171,    46,
      60,    45,   167,   154,    46,   172,   118,    76,    47,    48,
      55,    49,    50,    47,    48,   190,    49,    50,    41,   136,
      42,   157,   191,    74,    62,   152,    46,    65,   158,    75,
      77,    78,    79,    80,    63,    47,    48,    67,    49,    50,
      43,    51,    44,    55,    77,    78,    79,    80,    56,    79,
      80,   169,    64,   173,   118,    95,    96,    97,    98,   126,
     127,   128,   129,    68,    70,    82,   183,     1,     2,   101,
      71,    72,     3,     4,     5,     6,     7,     8,     9,    10,
      73,    81,    85,    11,    12,    13,    83,    88,    86,    87,
      14,    15,    89,    90,    91,    92,    99,   102,    16,   100,
      17,    55,   139,    18,   140,   141,   142,   143,   144,   145,
     146,   147,   149,   103,   140,   141,   142,   143,   144,   145,
     146,   147,   105,   113,   108,   109,   111,   112,   116,   122,
     125,   123,   131,   133,   170,   134,   174,   137,   155,   151,
     160,   161,   163,   168,   177,   178,   182,   184,   180,   186,
     187,   188,   192,    66,   181,   159,   165,   176,   189,   185,
      94,   153,   162,   115,   175,     0,   150
=======
      83,    58,    75,   146,   156,   174,    41,    60,    42,    77,
      61,   157,   175,    43,   129,    44,    93,    46,    62,    63,
      83,    64,    83,    65,    68,    69,    47,    48,   147,    50,
      51,    66,    71,   105,    72,   148,    96,    97,    98,    99,
      46,    83,    83,    83,    83,    45,   161,    73,    74,    47,
      48,    84,    50,    51,    79,    80,    81,    82,   130,   115,
      83,    76,    85,   144,   120,   121,   122,   123,    86,    87,
      46,    94,    88,    78,    89,    95,    90,   101,    46,    47,
      48,    49,    50,    51,    91,    52,    57,    47,    48,    45,
      50,    51,    79,    80,    81,    82,    46,    92,   168,   159,
     115,   102,   100,   104,    83,    47,    48,   106,    50,    51,
      79,    80,    81,    82,    46,   107,   109,   110,   111,   113,
     118,   119,   125,    47,    48,    49,    50,    51,   127,    52,
       1,     2,   128,   131,   143,     3,     4,     5,     6,     7,
       8,     9,    10,   145,   153,   151,    11,    12,    13,   150,
     162,   158,   165,    14,    15,   163,   149,   167,   169,   171,
     172,    16,   176,    17,    67,   166,    18,   170,   133,    46,
     134,   135,   136,   137,   138,   139,   140,   141,    47,    48,
      46,    50,    51,    79,    80,    81,    82,   155,   173,    47,
      48,     0,    50,    51,   152,   160,    81,    82
>>>>>>> expression
=======
      75,    58,   155,    46,    45,    81,    82,    77,    92,   156,
     173,   128,    47,    48,    78,    50,    51,   174,    60,   119,
     120,   121,   122,    61,    41,    45,    42,    64,    43,    46,
      44,   152,   145,    62,    95,    96,    97,    98,    47,    48,
      49,    50,    51,   160,    52,    57,    79,    80,    81,    82,
      46,    79,    80,    81,    82,    63,   114,   146,    65,    47,
      48,    49,    50,    51,   147,    52,    66,   129,    79,    80,
      81,    82,   143,    69,    68,    94,    76,     1,     2,   101,
      71,    83,     3,     4,     5,     6,     7,     8,     9,    10,
      72,    73,    74,    11,    12,    13,   158,   114,    84,    86,
      14,    15,    79,    80,    81,    82,    85,   167,    16,    89,
      17,    87,   132,    18,   133,   134,   135,   136,   137,   138,
     139,   140,    79,    80,    81,    82,    88,    79,    80,    81,
      82,    90,    91,    93,    99,   100,   101,   103,   110,   105,
     112,   106,   108,   109,   117,   118,   124,   126,   130,   144,
     127,   142,   149,   150,   164,   157,   166,   168,   162,   161,
     170,   165,   171,   175,    67,   148,   154,   172,   169,   104,
     151,   159
>>>>>>> add minus for expression
};

static const yytype_uint8 yycheck[] =
{
<<<<<<< HEAD
<<<<<<< HEAD
       4,    88,    18,   114,    53,    42,    17,    30,    42,    42,
       7,    17,    49,   124,    42,    49,   103,    19,    51,    52,
      53,    54,    55,    51,    52,    42,    54,    55,     6,   116,
       8,    42,    49,    45,    32,   122,    42,    38,    49,    51,
      56,    57,    58,    59,    53,    51,    52,     0,    54,    55,
       6,    57,     8,    53,    56,    57,    58,    59,    58,    58,
      59,   148,    53,   150,   151,    77,    78,    79,    80,    23,
      24,    25,    26,     3,    53,    32,   163,     4,     5,    83,
      53,    53,     9,    10,    11,    12,    13,    14,    15,    16,
      53,    29,    53,    20,    21,    22,    19,    43,    53,    35,
      27,    28,    39,    17,    36,    36,    53,    31,    35,    53,
      37,    53,    41,    40,    43,    44,    45,    46,    47,    48,
      49,    50,    41,    33,    43,    44,    45,    46,    47,    48,
      49,    50,    53,    19,    54,    53,    53,    53,    17,    43,
      30,    19,    19,    17,   148,    53,   150,    19,     6,    34,
      18,    53,    19,    50,    53,    51,    18,    18,    42,    42,
      42,    18,    42,    18,   159,   131,   137,   153,   183,   165,
      76,   123,   134,   101,   151,    -1,   119
=======
      55,     4,    45,    17,    42,    42,     6,    53,     8,    52,
       7,    49,    49,     6,   112,     8,    18,    42,    30,    32,
      75,    53,    77,    53,     0,     3,    51,    52,    42,    54,
      55,    38,    53,    88,    53,    49,    79,    80,    81,    82,
      42,    96,    97,    98,    99,    17,   144,    53,    53,    51,
      52,    32,    54,    55,    56,    57,    58,    59,   113,   102,
     115,    29,    53,   118,    23,    24,    25,    26,    53,    35,
      42,    53,    43,    19,    39,    78,    17,    31,    42,    51,
      52,    53,    54,    55,    36,    57,    58,    51,    52,    17,
      54,    55,    56,    57,    58,    59,    42,    36,   153,   142,
     143,    33,    53,    53,   159,    51,    52,    54,    54,    55,
      56,    57,    58,    59,    42,    53,    53,    53,    19,    17,
      43,    30,    19,    51,    52,    53,    54,    55,    17,    57,
       4,     5,    53,    19,    34,     9,    10,    11,    12,    13,
      14,    15,    16,     6,    19,    53,    20,    21,    22,    18,
      53,    50,    42,    27,    28,    51,   125,    18,    18,    42,
      18,    35,    42,    37,    18,   149,    40,   155,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      42,    54,    55,    56,    57,    58,    59,   131,   168,    51,
      52,    -1,    54,    55,   128,   143,    58,    59
>>>>>>> expression
=======
      45,     4,    42,    42,    17,    58,    59,    52,    18,    49,
      42,   111,    51,    52,    19,    54,    55,    49,    53,    23,
      24,    25,    26,     7,     6,    17,     8,    53,     6,    42,
       8,    19,    17,    30,    79,    80,    81,    82,    51,    52,
      53,    54,    55,   143,    57,    58,    56,    57,    58,    59,
      42,    56,    57,    58,    59,    32,   101,    42,    53,    51,
      52,    53,    54,    55,    49,    57,    38,   112,    56,    57,
      58,    59,   117,     3,     0,    78,    29,     4,     5,    33,
      53,    32,     9,    10,    11,    12,    13,    14,    15,    16,
      53,    53,    53,    20,    21,    22,   141,   142,    53,    35,
      27,    28,    56,    57,    58,    59,    53,   152,    35,    17,
      37,    43,    41,    40,    43,    44,    45,    46,    47,    48,
      49,    50,    56,    57,    58,    59,    39,    56,    57,    58,
      59,    36,    36,    53,    53,    31,    33,    53,    19,    54,
      17,    53,    53,    53,    43,    30,    19,    17,    19,     6,
      53,    34,    18,    53,    42,    50,    18,    18,    51,    53,
      42,   148,    18,    42,    18,   124,   130,   167,   154,    87,
     127,   142
>>>>>>> add minus for expression
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     4,     5,     9,    10,    11,    12,    13,    14,    15,
      16,    20,    21,    22,    27,    28,    35,    37,    40,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
<<<<<<< HEAD
      73,    74,    75,    80,    85,    86,    89,    90,   101,   102,
     103,     6,     8,     6,     8,    17,    42,    51,    52,    54,
      55,    57,    84,    91,    92,    53,    58,    93,    94,    53,
       7,    30,    32,    53,    53,    38,    63,     0,     3,   104,
      53,    53,    53,    53,    92,    92,    19,    56,    57,    58,
      59,    29,    32,    19,    95,    53,    53,    35,    43,    39,
      17,    36,    36,    18,    91,    92,    92,    92,    92,    53,
      53,    94,    31,    33,    97,    53,    88,    84,    54,    53,
      77,    53,    53,    19,    96,    95,    17,    82,    84,    94,
      98,    99,    43,    19,    87,    30,    23,    24,    25,    26,
      79,    19,    76,    17,    53,    97,    84,    19,    81,    41,
      43,    44,    45,    46,    47,    48,    49,    50,   100,    41,
     100,    34,    84,    88,    97,     6,    17,    42,    49,    77,
      18,    53,    96,    19,    83,    82,    42,    49,    50,    84,
      94,    42,    49,    84,    94,    98,    87,    53,    51,    78,
      42,    76,    18,    84,    18,    81,    42,    42,    18,    83,
      42,    49,    42
=======
      73,    74,    75,    80,    85,    86,    87,    88,    98,    99,
     100,     6,     8,     6,     8,    17,    42,    51,    52,    53,
      54,    55,    57,    84,    89,    90,    92,    58,    89,    91,
      53,     7,    30,    32,    53,    53,    38,    63,     0,     3,
     101,    53,    53,    53,    53,    90,    29,    90,    19,    56,
<<<<<<< HEAD
      57,    58,    59,    84,    32,    53,    53,    35,    43,    39,
      17,    36,    36,    18,    53,    89,    90,    90,    90,    90,
      53,    31,    33,    94,    53,    84,    54,    53,    77,    53,
      53,    19,    93,    17,    82,    90,    95,    96,    43,    30,
      23,    24,    25,    26,    79,    19,    76,    17,    53,    94,
      84,    19,    81,    41,    43,    44,    45,    46,    47,    48,
      49,    50,    97,    34,    84,     6,    17,    42,    49,    77,
      18,    53,    93,    19,    83,    82,    42,    49,    50,    90,
      95,    94,    53,    51,    78,    42,    76,    18,    84,    18,
      81,    42,    18,    83,    42,    49,    42
>>>>>>> expression
=======
      57,    58,    59,    32,    53,    53,    35,    43,    39,    17,
      36,    36,    18,    53,    89,    90,    90,    90,    90,    53,
      31,    33,    94,    53,    84,    54,    53,    77,    53,    53,
      19,    93,    17,    82,    90,    95,    96,    43,    30,    23,
      24,    25,    26,    79,    19,    76,    17,    53,    94,    90,
      19,    81,    41,    43,    44,    45,    46,    47,    48,    49,
      50,    97,    34,    90,     6,    17,    42,    49,    77,    18,
      53,    93,    19,    83,    82,    42,    49,    50,    90,    95,
      94,    53,    51,    78,    42,    76,    18,    90,    18,    81,
      42,    18,    83,    42,    49,    42
>>>>>>> add minus for expression
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    61,    62,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    76,    77,    77,    77,
      77,    77,    77,    78,    79,    79,    79,    79,    80,    81,
      81,    82,    83,    83,    84,    84,    84,    84,    84,    85,
<<<<<<< HEAD
      86,    87,    87,    88,    89,    90,    91,    91,    92,    92,
      92,    92,    92,    92,    92,    93,    93,    94,    94,    95,
      95,    96,    96,    97,    97,    98,    98,    99,    99,    99,
      99,    99,    99,    99,    99,   100,   100,   100,   100,   100,
     100,   100,   100,   101,   102,   103,   104,   104
=======
      86,    87,    88,    89,    89,    90,    90,    90,    90,    90,
<<<<<<< HEAD
      90,    90,    90,    90,    91,    91,    92,    92,    93,    93,
      94,    94,    95,    95,    96,    96,    96,    97,    97,    97,
      97,    97,    97,    97,    97,    98,    99,   100,   101,   101
>>>>>>> expression
=======
      90,    90,    90,    91,    91,    92,    92,    93,    93,    94,
      94,    95,    95,    96,    96,    96,    97,    97,    97,    97,
      97,    97,    97,    97,    98,    99,   100,   101,   101
>>>>>>> add minus for expression
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     2,     8,     5,     7,     0,     3,     5,     7,     6,
       2,     4,     3,     1,     1,     1,     1,     1,     6,     0,
       3,     4,     0,     3,     1,     1,     1,     1,     1,     4,
<<<<<<< HEAD
       6,     0,     3,     3,     6,     2,     1,     3,     3,     3,
       3,     3,     3,     2,     1,     1,     2,     1,     3,     0,
       3,     0,     3,     0,     2,     1,     3,     3,     3,     3,
       3,     4,     3,     4,     3,     1,     1,     1,     1,     1,
       1,     1,     2,     7,     2,     4,     0,     1
=======
       7,     6,     2,     1,     3,     3,     3,     3,     3,     3,
<<<<<<< HEAD
       2,     1,     2,     1,     1,     1,     1,     3,     0,     3,
       0,     2,     1,     3,     3,     3,     4,     1,     1,     1,
       1,     1,     1,     1,     2,     7,     2,     4,     0,     1
>>>>>>> expression
=======
       2,     1,     1,     1,     1,     1,     3,     0,     3,     0,
       2,     1,     3,     3,     3,     4,     1,     1,     1,     1,
       1,     1,     1,     2,     7,     2,     4,     0,     1
>>>>>>> add minus for expression
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, sql_string, sql_result, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, sql_string, sql_result, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  YYUSE (sql_string);
  YYUSE (sql_result);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yytype, yyvaluep, yylocationp, sql_string, sql_result, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , sql_string, sql_result, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, sql_string, sql_result, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (sql_string);
  YYUSE (sql_result);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 190 "yacc_sql.y"
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1692 "yacc_sql.cpp"
=======
#line 1684 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1680 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 23:
#line 220 "yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1701 "yacc_sql.cpp"
=======
#line 1693 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1689 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 24:
#line 226 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1709 "yacc_sql.cpp"
=======
#line 1701 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1697 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 25:
#line 231 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1717 "yacc_sql.cpp"
=======
#line 1709 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1705 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 26:
#line 237 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1725 "yacc_sql.cpp"
=======
#line 1717 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1713 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 27:
#line 243 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1733 "yacc_sql.cpp"
=======
#line 1725 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1721 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 28:
#line 249 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1741 "yacc_sql.cpp"
=======
#line 1733 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1729 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 29:
#line 255 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1751 "yacc_sql.cpp"
=======
#line 1743 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1739 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 30:
#line 262 "yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1759 "yacc_sql.cpp"
=======
#line 1751 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1747 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 31:
#line 268 "yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1769 "yacc_sql.cpp"
=======
#line 1761 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1757 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 32:
#line 277 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-5].string);
      create_index.relation_name = (yyvsp[-3].string);
      create_index.attribute_name = (yyvsp[-1].string);
      free((yyvsp[-5].string));
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1784 "yacc_sql.cpp"
=======
#line 1776 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1772 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 33:
#line 291 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].string);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1796 "yacc_sql.cpp"
=======
#line 1788 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1784 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 34:
#line 301 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-4].string);
      free((yyvsp[-4].string));

      std::vector<AttrInfoSqlNode> *src_attrs = (yyvsp[-1].attr_infos);

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*(yyvsp[-2].attr_info));
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete (yyvsp[-2].attr_info);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1816 "yacc_sql.cpp"
=======
#line 1808 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1804 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 35:
#line 319 "yacc_sql.y"
    {
      (yyval.attr_infos) = nullptr;
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1824 "yacc_sql.cpp"
=======
#line 1816 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1812 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 36:
#line 323 "yacc_sql.y"
    {
      if ((yyvsp[0].attr_infos) != nullptr) {
        (yyval.attr_infos) = (yyvsp[0].attr_infos);
      } else {
        (yyval.attr_infos) = new std::vector<AttrInfoSqlNode>;
      }
      (yyval.attr_infos)->emplace_back(*(yyvsp[-1].attr_info));
      delete (yyvsp[-1].attr_info);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1838 "yacc_sql.cpp"
=======
#line 1830 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1826 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 37:
#line 336 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].string);
      (yyval.attr_info)->length = (yyvsp[-1].number);
      (yyval.attr_info)->nullable = false;
      free((yyvsp[-4].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1851 "yacc_sql.cpp"
=======
#line 1843 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1839 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 38:
#line 345 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-5].number);
      (yyval.attr_info)->name = (yyvsp[-6].string);
      (yyval.attr_info)->length = (yyvsp[-3].number);
      (yyval.attr_info)->nullable = false;
      free((yyvsp[-6].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1864 "yacc_sql.cpp"
=======
#line 1856 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1852 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 39:
#line 354 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].string);
      (yyval.attr_info)->length = (yyvsp[-2].number);
      (yyval.attr_info)->nullable = true;
      free((yyvsp[-5].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1877 "yacc_sql.cpp"
=======
#line 1869 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1865 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 40:
#line 363 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[0].number);
      (yyval.attr_info)->name = (yyvsp[-1].string);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->nullable = false;
      free((yyvsp[-1].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1890 "yacc_sql.cpp"
=======
#line 1882 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1878 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 41:
#line 372 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-2].number);
      (yyval.attr_info)->name = (yyvsp[-3].string);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->nullable = false;
      free((yyvsp[-3].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1903 "yacc_sql.cpp"
=======
#line 1895 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1891 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 42:
#line 381 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-1].number);
      (yyval.attr_info)->name = (yyvsp[-2].string);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->nullable = true;
      free((yyvsp[-2].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1916 "yacc_sql.cpp"
=======
#line 1908 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1904 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 43:
#line 391 "yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
<<<<<<< HEAD
<<<<<<< HEAD
#line 1922 "yacc_sql.cpp"
=======
#line 1914 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1910 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 44:
#line 394 "yacc_sql.y"
               { (yyval.number)=INTS; }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1928 "yacc_sql.cpp"
=======
#line 1920 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1916 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 45:
#line 395 "yacc_sql.y"
               { (yyval.number)=CHARS; }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1934 "yacc_sql.cpp"
=======
#line 1926 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1922 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 46:
#line 396 "yacc_sql.y"
               { (yyval.number)=FLOATS; }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1940 "yacc_sql.cpp"
=======
#line 1932 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1928 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 47:
#line 397 "yacc_sql.y"
               { (yyval.number)=DATES;}
<<<<<<< HEAD
<<<<<<< HEAD
#line 1946 "yacc_sql.cpp"
=======
#line 1938 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1934 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 48:
#line 401 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-3].string);
      if ((yyvsp[0].insert_value_list) != nullptr) {
        (yyval.sql_node)->insertion.values.swap(*(yyvsp[0].insert_value_list));
      }
      (yyval.sql_node)->insertion.values.emplace_back(*(yyvsp[-1].value_list));
      std::reverse((yyval.sql_node)->insertion.values.begin(), (yyval.sql_node)->insertion.values.end());
      delete (yyvsp[-1].value_list);
      free((yyvsp[-3].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1962 "yacc_sql.cpp"
=======
#line 1954 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1950 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 49:
#line 416 "yacc_sql.y"
    {
      (yyval.insert_value_list) = nullptr;
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1970 "yacc_sql.cpp"
=======
#line 1962 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1958 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 50:
#line 420 "yacc_sql.y"
    {
      if ((yyvsp[0].insert_value_list) != nullptr) {
        (yyval.insert_value_list) = (yyvsp[0].insert_value_list);
      } else {
        (yyval.insert_value_list) = new std::vector<std::vector<Value>>;
      }
      (yyval.insert_value_list)->emplace_back(*(yyvsp[-1].value_list));
      delete (yyvsp[-1].value_list);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1984 "yacc_sql.cpp"
=======
#line 1976 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 1972 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 51:
#line 433 "yacc_sql.y"
    {
      Value tmp;
      if(!exp2value((yyvsp[-2].expression),tmp))
      {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "error");
        YYERROR;
      }
      if ((yyvsp[-1].value_list) != nullptr) {
        (yyval.value_list) = (yyvsp[-1].value_list);
      } else {
        (yyval.value_list) = new std::vector<Value>;
      }
      (yyval.value_list)->emplace_back(tmp);
      std::reverse((yyval.value_list)->begin(), (yyval.value_list)->end());
      delete (yyvsp[-2].expression);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 1999 "yacc_sql.cpp"
=======
#line 1991 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 52:
#line 447 "yacc_sql.y"
    {
      (yyval.value_list) = nullptr;
    }
<<<<<<< HEAD
#line 2007 "yacc_sql.cpp"
=======
#line 1999 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 53:
#line 450 "yacc_sql.y"
                              { 
=======
#line 1993 "yacc_sql.cpp"
    break;

  case 52:
#line 449 "yacc_sql.y"
    {
      (yyval.value_list) = nullptr;
    }
#line 2001 "yacc_sql.cpp"
    break;

  case 53:
#line 452 "yacc_sql.y"
                                   { 
      Value tmp;
      if(!exp2value((yyvsp[-1].expression),tmp))
      {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "error");
        YYERROR;
      }
>>>>>>> add minus for expression
      if ((yyvsp[0].value_list) != nullptr) {
        (yyval.value_list) = (yyvsp[0].value_list);
      } else {
        (yyval.value_list) = new std::vector<Value>;
      }
      (yyval.value_list)->emplace_back(tmp);
      delete (yyvsp[-1].expression);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2021 "yacc_sql.cpp"
=======
#line 2013 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 54:
#line 461 "yacc_sql.y"
=======
#line 2021 "yacc_sql.cpp"
    break;

  case 54:
#line 469 "yacc_sql.y"
>>>>>>> add minus for expression
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]); // useless
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2030 "yacc_sql.cpp"
=======
#line 2022 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 55:
#line 465 "yacc_sql.y"
=======
#line 2030 "yacc_sql.cpp"
    break;

  case 55:
#line 473 "yacc_sql.y"
>>>>>>> add minus for expression
           {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]); // useless
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2039 "yacc_sql.cpp"
=======
#line 2031 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 56:
#line 469 "yacc_sql.y"
=======
#line 2039 "yacc_sql.cpp"
    break;

  case 56:
#line 477 "yacc_sql.y"
>>>>>>> add minus for expression
              {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      std::string str(tmp);
      Value * value = new Value();
      int date;
      if(string_to_date(str,date) < 0)
      {
        yyerror(&(yyloc),sql_string,sql_result,scanner,"date invaid",true);
        YYERROR;
      }
      else
      {
        value->set_date(date);
      }
      (yyval.value) = value;
      free(tmp);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2061 "yacc_sql.cpp"
=======
#line 2053 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 57:
#line 486 "yacc_sql.y"
=======
#line 2061 "yacc_sql.cpp"
    break;

  case 57:
#line 494 "yacc_sql.y"
>>>>>>> add minus for expression
         {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value) = new Value(tmp);
      free(tmp);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2071 "yacc_sql.cpp"
=======
#line 2063 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 58:
#line 491 "yacc_sql.y"
=======
#line 2071 "yacc_sql.cpp"
    break;

  case 58:
#line 499 "yacc_sql.y"
>>>>>>> add minus for expression
             {
      (yyval.value) = new Value();
      (yyval.value)->set_null();
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2080 "yacc_sql.cpp"
=======
#line 2072 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 59:
#line 499 "yacc_sql.y"
=======
#line 2080 "yacc_sql.cpp"
    break;

  case 59:
#line 507 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-1].string);
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->deletion.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      free((yyvsp[-1].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2094 "yacc_sql.cpp"
=======
#line 2086 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 60:
#line 511 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      (yyval.sql_node)->update.relation_name = (yyvsp[-4].string);
      (yyval.sql_node)->update.attribute_names.emplace_back((yyvsp[-2].update_kv)->attr_name);
      (yyval.sql_node)->update.values.emplace_back((yyvsp[-2].update_kv)->value);
      if (nullptr != (yyvsp[-1].update_kv_list)) {
        for (UpdateKV kv : *(yyvsp[-1].update_kv_list)) {
          (yyval.sql_node)->update.attribute_names.emplace_back(kv.attr_name);
          (yyval.sql_node)->update.values.emplace_back(kv.value);
        }
        delete (yyvsp[-1].update_kv_list);
      }
  
=======
#line 2094 "yacc_sql.cpp"
    break;

  case 60:
#line 519 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      (yyval.sql_node)->update.relation_name = (yyvsp[-5].string);
      (yyval.sql_node)->update.attribute_name = (yyvsp[-3].string);
      Value tmp;
      if(!exp2value((yyvsp[-1].expression),tmp))
      {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "error");
        YYERROR;
      }
      (yyval.sql_node)->update.value = tmp;
>>>>>>> add minus for expression
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->update.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
<<<<<<< HEAD
      free((yyvsp[-4].string));
      delete (yyvsp[-2].update_kv);
    }
<<<<<<< HEAD
#line 2119 "yacc_sql.cpp"
=======
#line 2104 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 61:
#line 534 "yacc_sql.y"
    {
      (yyval.update_kv_list) = nullptr;
    }
#line 2127 "yacc_sql.cpp"
    break;

  case 62:
#line 538 "yacc_sql.y"
    {
      if ((yyvsp[0].update_kv_list) != nullptr) {
        (yyval.update_kv_list) = (yyvsp[0].update_kv_list);
      } else {
        (yyval.update_kv_list) = new std::vector<UpdateKV>;
      }
      (yyval.update_kv_list)->emplace_back(*(yyvsp[-1].update_kv));
      delete (yyvsp[-1].update_kv);
    }
#line 2141 "yacc_sql.cpp"
    break;

  case 63:
#line 550 "yacc_sql.y"
    {
      (yyval.update_kv) = new UpdateKV;
      (yyval.update_kv)->attr_name = (yyvsp[-2].string);
      (yyval.update_kv)->value = *(yyvsp[0].value);
    }
#line 2151 "yacc_sql.cpp"
    break;

  case 64:
#line 559 "yacc_sql.y"
=======
      free((yyvsp[-5].string));
      free((yyvsp[-3].string));
      delete (yyvsp[-1].expression);
    }
#line 2118 "yacc_sql.cpp"
    break;

  case 61:
#line 541 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-4].expression_list) != nullptr) {
        (yyval.sql_node)->selection.project_exprs.swap(*(yyvsp[-4].expression_list));
        delete (yyvsp[-4].expression_list);
      }
      if ((yyvsp[-1].relation_list) != nullptr) {
        (yyval.sql_node)->selection.relations.swap(*(yyvsp[-1].relation_list));
        delete (yyvsp[-1].relation_list);
      }
      (yyval.sql_node)->selection.relations.push_back((yyvsp[-2].string));
      std::reverse((yyval.sql_node)->selection.relations.begin(), (yyval.sql_node)->selection.relations.end());

      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->selection.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      free((yyvsp[-2].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2175 "yacc_sql.cpp"
=======
#line 2128 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 65:
#line 581 "yacc_sql.y"
=======
#line 2142 "yacc_sql.cpp"
    break;

  case 62:
#line 563 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2186 "yacc_sql.cpp"
=======
#line 2139 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 66:
#line 591 "yacc_sql.y"
=======
#line 2153 "yacc_sql.cpp"
    break;

  case 63:
#line 573 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      (yyval.expression_list) = new std::vector<Expression*>;
      (yyval.expression_list)->emplace_back((yyvsp[0].expression));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2195 "yacc_sql.cpp"
=======
#line 2148 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 67:
#line 596 "yacc_sql.y"
=======
#line 2162 "yacc_sql.cpp"
    break;

  case 64:
#line 578 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2208 "yacc_sql.cpp"
=======
#line 2161 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 68:
#line 606 "yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
<<<<<<< HEAD
#line 2216 "yacc_sql.cpp"
=======
#line 2169 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 69:
#line 609 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
<<<<<<< HEAD
#line 2224 "yacc_sql.cpp"
=======
#line 2177 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 70:
#line 612 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
<<<<<<< HEAD
#line 2232 "yacc_sql.cpp"
=======
#line 2185 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 71:
#line 615 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
<<<<<<< HEAD
#line 2240 "yacc_sql.cpp"
=======
#line 2193 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 72:
#line 618 "yacc_sql.y"
=======
#line 2175 "yacc_sql.cpp"
    break;

  case 65:
#line 588 "yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2183 "yacc_sql.cpp"
    break;

  case 66:
#line 591 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2191 "yacc_sql.cpp"
    break;

  case 67:
#line 594 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2199 "yacc_sql.cpp"
    break;

  case 68:
#line 597 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2207 "yacc_sql.cpp"
    break;

  case 69:
#line 600 "yacc_sql.y"
>>>>>>> add minus for expression
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
<<<<<<< HEAD
<<<<<<< HEAD
#line 2249 "yacc_sql.cpp"
=======
#line 2202 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 73:
#line 622 "yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
<<<<<<< HEAD
#line 2257 "yacc_sql.cpp"
=======
#line 2210 "yacc_sql.cpp"
>>>>>>> expression
    break;

  case 74:
#line 625 "yacc_sql.y"
=======
#line 2216 "yacc_sql.cpp"
    break;

  case 70:
#line 604 "yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 2224 "yacc_sql.cpp"
    break;

  case 71:
#line 607 "yacc_sql.y"
>>>>>>> add minus for expression
            {
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2271 "yacc_sql.cpp"
    break;

  case 75:
#line 637 "yacc_sql.y"
        {
      (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      RelAttrSqlNode attr;
      attr.relation_name  = "";
      attr.attribute_name = "*";
      (yyval.rel_attr_list)->emplace_back(attr);
    }
#line 2283 "yacc_sql.cpp"
    break;

  case 76:
#line 644 "yacc_sql.y"
                         {
      if ((yyvsp[0].rel_attr_list) != nullptr) {
        (yyval.rel_attr_list) = (yyvsp[0].rel_attr_list);
      } else {
        (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      }
      (yyval.rel_attr_list)->emplace_back(*(yyvsp[-1].rel_attr));
      delete (yyvsp[-1].rel_attr);
    }
#line 2297 "yacc_sql.cpp"
    break;

  case 77:
#line 656 "yacc_sql.y"
=======
#line 2224 "yacc_sql.cpp"
=======
#line 2220 "yacc_sql.cpp"
>>>>>>> expression support null
=======
#line 2234 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 72:
#line 623 "yacc_sql.y"
              {
      FieldExpr *tmp = new FieldExpr();
      tmp->set_table_name((yyvsp[0].rel_attr)->relation_name);
      tmp->set_field_name((yyvsp[0].rel_attr)->attribute_name);
      (yyval.expression) = tmp;
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].rel_attr);
    }
#line 2247 "yacc_sql.cpp"
    break;

  case 73:
#line 653 "yacc_sql.y"
        {
      (yyval.expression_list) = new std::vector<Expression *>;
      FieldExpr *expr = new FieldExpr();
      expr->set_table_name("");
      expr->set_field_name("*");
      (yyval.expression_list)->emplace_back(expr);
    }
#line 2259 "yacc_sql.cpp"
    break;

  case 74:
#line 660 "yacc_sql.y"
                     {
      (yyval.expression_list) = (yyvsp[0].expression_list);
    }
#line 2267 "yacc_sql.cpp"
    break;

<<<<<<< HEAD
  case 76:
<<<<<<< HEAD
#line 652 "yacc_sql.y"
>>>>>>> expression
=======
#line 648 "yacc_sql.y"
>>>>>>> expression support null
=======
  case 75:
#line 666 "yacc_sql.y"
>>>>>>> add minus for expression
       {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2307 "yacc_sql.cpp"
    break;

  case 78:
#line 661 "yacc_sql.y"
=======
#line 2283 "yacc_sql.cpp"
    break;

  case 77:
#line 657 "yacc_sql.y"
>>>>>>> expression
=======
#line 2279 "yacc_sql.cpp"
    break;

  case 77:
#line 653 "yacc_sql.y"
>>>>>>> expression support null
=======
#line 2277 "yacc_sql.cpp"
    break;

  case 76:
#line 671 "yacc_sql.y"
>>>>>>> add minus for expression
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2319 "yacc_sql.cpp"
    break;

  case 79:
#line 672 "yacc_sql.y"
    {
      (yyval.rel_attr_list) = nullptr;
    }
#line 2327 "yacc_sql.cpp"
    break;

  case 80:
#line 675 "yacc_sql.y"
                               {
      if ((yyvsp[0].rel_attr_list) != nullptr) {
        (yyval.rel_attr_list) = (yyvsp[0].rel_attr_list);
      } else {
        (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      }

      (yyval.rel_attr_list)->emplace_back(*(yyvsp[-1].rel_attr));
      delete (yyvsp[-1].rel_attr);
    }
#line 2342 "yacc_sql.cpp"
    break;

  case 81:
#line 689 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 2350 "yacc_sql.cpp"
    break;

  case 82:
#line 692 "yacc_sql.y"
=======
#line 2295 "yacc_sql.cpp"
=======
#line 2291 "yacc_sql.cpp"
>>>>>>> expression support null
=======
#line 2289 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 77:
#line 699 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 2297 "yacc_sql.cpp"
    break;

<<<<<<< HEAD
  case 79:
<<<<<<< HEAD
#line 688 "yacc_sql.y"
>>>>>>> expression
=======
#line 684 "yacc_sql.y"
>>>>>>> expression support null
=======
  case 78:
#line 702 "yacc_sql.y"
>>>>>>> add minus for expression
                        {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }

      (yyval.relation_list)->push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2365 "yacc_sql.cpp"
    break;

  case 83:
#line 705 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2373 "yacc_sql.cpp"
    break;

  case 84:
#line 708 "yacc_sql.y"
                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);  
    }
#line 2381 "yacc_sql.cpp"
    break;

  case 85:
#line 713 "yacc_sql.y"
=======
#line 2318 "yacc_sql.cpp"
=======
#line 2314 "yacc_sql.cpp"
>>>>>>> expression support null
=======
#line 2312 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 79:
#line 715 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2320 "yacc_sql.cpp"
    break;

  case 80:
#line 718 "yacc_sql.y"
                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);  
    }
#line 2328 "yacc_sql.cpp"
    break;

<<<<<<< HEAD
  case 82:
<<<<<<< HEAD
#line 709 "yacc_sql.y"
>>>>>>> expression
=======
#line 705 "yacc_sql.y"
>>>>>>> expression support null
=======
  case 81:
#line 723 "yacc_sql.y"
>>>>>>> add minus for expression
              {
      (yyval.condition_list) = new std::vector<ConditionSqlNode>;
      (yyval.condition_list)->emplace_back(*(yyvsp[0].condition));
      delete (yyvsp[0].condition);
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2391 "yacc_sql.cpp"
    break;

  case 86:
#line 718 "yacc_sql.y"
=======
#line 2344 "yacc_sql.cpp"
    break;

  case 83:
#line 714 "yacc_sql.y"
>>>>>>> expression
=======
#line 2340 "yacc_sql.cpp"
    break;

  case 83:
#line 710 "yacc_sql.y"
>>>>>>> expression support null
=======
#line 2338 "yacc_sql.cpp"
    break;

  case 82:
#line 728 "yacc_sql.y"
>>>>>>> add minus for expression
                                   {
      (yyval.condition_list) = (yyvsp[0].condition_list);
      (yyval.condition_list)->emplace_back(*(yyvsp[-2].condition));
      delete (yyvsp[-2].condition);
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2401 "yacc_sql.cpp"
    break;

  case 87:
#line 726 "yacc_sql.y"
=======
#line 2354 "yacc_sql.cpp"
    break;

  case 84:
#line 722 "yacc_sql.y"
>>>>>>> expression
=======
#line 2350 "yacc_sql.cpp"
    break;

  case 84:
#line 718 "yacc_sql.y"
>>>>>>> expression support null
=======
#line 2348 "yacc_sql.cpp"
    break;

  case 83:
#line 736 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_expr = (yyvsp[-2].expression);
      (yyval.condition)->right_expr = (yyvsp[0].expression);
      (yyval.condition)->comp = (yyvsp[-1].comp);
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2417 "yacc_sql.cpp"
    break;

  case 88:
#line 738 "yacc_sql.y"
=======
#line 2365 "yacc_sql.cpp"
    break;

  case 85:
#line 729 "yacc_sql.y"
>>>>>>> expression
=======
#line 2361 "yacc_sql.cpp"
    break;

  case 85:
#line 725 "yacc_sql.y"
>>>>>>> expression support null
=======
#line 2359 "yacc_sql.cpp"
    break;

  case 84:
#line 743 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_expr = (yyvsp[-2].expression);
      (yyval.condition)->comp = IS_NULL;
      ValueExpr *value_expr = new ValueExpr();
      Value val;
      val.set_null();
      value_expr->set_value(val);
      (yyval.condition)->right_expr = value_expr;
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2433 "yacc_sql.cpp"
    break;

  case 89:
#line 750 "yacc_sql.y"
=======
#line 2380 "yacc_sql.cpp"
    break;

  case 86:
#line 740 "yacc_sql.y"
>>>>>>> expression
=======
#line 2376 "yacc_sql.cpp"
    break;

  case 86:
#line 736 "yacc_sql.y"
>>>>>>> expression support null
=======
#line 2374 "yacc_sql.cpp"
    break;

  case 85:
#line 754 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_expr = (yyvsp[-3].expression);
      (yyval.condition)->comp = IS_NOT_NULL;
      ValueExpr *value_expr = new ValueExpr();
      Value val;
      val.set_null();
      value_expr->set_value(val);
      (yyval.condition)->right_expr = value_expr;
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2449 "yacc_sql.cpp"
    break;

  case 90:
#line 762 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-2].value);
      (yyval.condition)->right_is_attr = 1;
      (yyval.condition)->right_attr = *(yyvsp[0].rel_attr);
      (yyval.condition)->comp = (yyvsp[-1].comp);

      delete (yyvsp[-2].value);
      
      delete (yyvsp[0].rel_attr);
    }
#line 2466 "yacc_sql.cpp"
    break;

  case 91:
#line 775 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-3].rel_attr);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->right_value.set_null();
      (yyval.condition)->comp = CompOp::IS_NOT_NULL;
      delete (yyvsp[-3].rel_attr);
    }
#line 2480 "yacc_sql.cpp"
    break;

  case 92:
#line 785 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-2].rel_attr);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->right_value.set_null();
      (yyval.condition)->comp = CompOp::IS_NULL;
      delete (yyvsp[-2].rel_attr);
    }
#line 2494 "yacc_sql.cpp"
    break;

  case 93:
#line 795 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-3].value);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->right_value.set_null();
      (yyval.condition)->comp = CompOp::IS_NOT_NULL;
      delete (yyvsp[-3].value);
    }
#line 2508 "yacc_sql.cpp"
    break;

  case 94:
#line 805 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-2].value);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->right_value.set_null();
      (yyval.condition)->comp = CompOp::IS_NULL;
      delete (yyvsp[-2].value);
    }
#line 2522 "yacc_sql.cpp"
    break;

  case 95:
#line 817 "yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 2528 "yacc_sql.cpp"
    break;

  case 96:
#line 818 "yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 2534 "yacc_sql.cpp"
    break;

  case 97:
#line 819 "yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 2540 "yacc_sql.cpp"
    break;

  case 98:
#line 820 "yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 2546 "yacc_sql.cpp"
    break;

  case 99:
#line 821 "yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 2552 "yacc_sql.cpp"
    break;

  case 100:
#line 822 "yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 2558 "yacc_sql.cpp"
    break;

  case 101:
#line 823 "yacc_sql.y"
           { (yyval.comp) = LIKE_OP;}
#line 2564 "yacc_sql.cpp"
    break;

  case 102:
#line 824 "yacc_sql.y"
               {(yyval.comp) = NOT_LIKE_OP;}
#line 2570 "yacc_sql.cpp"
    break;

  case 103:
#line 829 "yacc_sql.y"
=======
#line 2395 "yacc_sql.cpp"
=======
#line 2391 "yacc_sql.cpp"
>>>>>>> expression support null
=======
#line 2389 "yacc_sql.cpp"
>>>>>>> add minus for expression
    break;

  case 86:
#line 856 "yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 2395 "yacc_sql.cpp"
    break;

  case 87:
#line 857 "yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 2401 "yacc_sql.cpp"
    break;

  case 88:
#line 858 "yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 2407 "yacc_sql.cpp"
    break;

  case 89:
#line 859 "yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 2413 "yacc_sql.cpp"
    break;

  case 90:
#line 860 "yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 2419 "yacc_sql.cpp"
    break;

  case 91:
#line 861 "yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 2425 "yacc_sql.cpp"
    break;

  case 92:
#line 862 "yacc_sql.y"
           { (yyval.comp) = LIKE_OP;}
#line 2431 "yacc_sql.cpp"
    break;

  case 93:
#line 863 "yacc_sql.y"
               {(yyval.comp) = NOT_LIKE_OP;}
#line 2437 "yacc_sql.cpp"
    break;

<<<<<<< HEAD
  case 95:
<<<<<<< HEAD
#line 854 "yacc_sql.y"
>>>>>>> expression
=======
#line 850 "yacc_sql.y"
>>>>>>> expression support null
=======
  case 94:
#line 868 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      char *tmp_file_name = common::substr((yyvsp[-3].string), 1, strlen((yyvsp[-3].string)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].string);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free((yyvsp[0].string));
      free(tmp_file_name);
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2584 "yacc_sql.cpp"
    break;

  case 104:
#line 842 "yacc_sql.y"
=======
#line 2457 "yacc_sql.cpp"
    break;

  case 96:
#line 867 "yacc_sql.y"
>>>>>>> expression
=======
#line 2453 "yacc_sql.cpp"
    break;

  case 96:
#line 863 "yacc_sql.y"
>>>>>>> expression support null
=======
#line 2451 "yacc_sql.cpp"
    break;

  case 95:
#line 881 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2593 "yacc_sql.cpp"
    break;

  case 105:
#line 850 "yacc_sql.y"
=======
#line 2466 "yacc_sql.cpp"
    break;

  case 97:
#line 875 "yacc_sql.y"
>>>>>>> expression
=======
#line 2462 "yacc_sql.cpp"
    break;

  case 97:
#line 871 "yacc_sql.y"
>>>>>>> expression support null
=======
#line 2460 "yacc_sql.cpp"
    break;

  case 96:
#line 889 "yacc_sql.y"
>>>>>>> add minus for expression
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].string);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      free((yyvsp[-2].string));
      delete (yyvsp[0].value);
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 2605 "yacc_sql.cpp"
    break;


#line 2609 "yacc_sql.cpp"
=======
#line 2478 "yacc_sql.cpp"
    break;


#line 2482 "yacc_sql.cpp"
>>>>>>> expression
=======
#line 2474 "yacc_sql.cpp"
    break;


#line 2478 "yacc_sql.cpp"
>>>>>>> expression support null
=======
#line 2472 "yacc_sql.cpp"
    break;


#line 2476 "yacc_sql.cpp"
>>>>>>> add minus for expression

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, sql_string, sql_result, scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, sql_string, sql_result, scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, sql_string, sql_result, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#line 862 "yacc_sql.y"
=======
#line 887 "yacc_sql.y"
>>>>>>> expression
=======
#line 883 "yacc_sql.y"
>>>>>>> expression support null
=======
#line 901 "yacc_sql.y"
>>>>>>> add minus for expression

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, ParsedSqlResult *sql_result) {
  yyscan_t scanner;
  yylex_init(&scanner);
  scan_string(s, scanner);
  int result = yyparse(s, sql_result, scanner);
  yylex_destroy(scanner);
  return result;
}
