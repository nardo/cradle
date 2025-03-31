/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse kt_parse
#define yylex   kt_lex
#define yyerror kt_error
#define yylval  kt_lval
#define yychar  kt_char
#define yydebug kt_debug
#define yynerrs kt_nerrs
#define yylloc kt_lloc

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     START_TOK_SCRIPT = 258,
     START_TOK_GDECL = 259,
     rwSTATE = 260,
     rwBETWEEN = 261,
     rwOBJECT = 262,
     rwCLASS = 263,
     rwRECORD = 264,
     rwDIRECTORY = 265,
     rwCONNECTION = 266,
     rwALIAS = 267,
     rwFACET = 268,
     rwIN = 269,
     rwFROM = 270,
     rwTO = 271,
     rwVIA = 272,
     rwTRY = 273,
     rwCATCH = 274,
     rwFINALLY = 275,
     rwWHEN = 276,
     rwFUNCTION = 277,
     rwMETHOD = 278,
     rwRETURN = 279,
     rwWHILE = 280,
     rwFOR = 281,
     rwBREAK = 282,
     rwCONTINUE = 283,
     rwIF = 284,
     rwELSE = 285,
     rwSWITCH = 286,
     rwCASE = 287,
     rwDEFAULT = 288,
     rwVAR = 289,
     rwINTEGER = 290,
     rwFLOAT = 291,
     rwBOOL = 292,
     rwSTRING = 293,
     rwSHARED = 294,
     rwPUBLIC = 295,
     rwFILE = 296,
     ILLEGAL_TOKEN = 297,
     INT_CONSTANT = 298,
     IDENTIFIER = 299,
     STRING_CONSTANT = 300,
     FLOAT_CONSTANT = 301,
     PATH = 302,
     STRING_FRAGMENT = 303,
     STRING_END = 304,
     INDENT = 305,
     DEDENT = 306,
     opARROW = 307,
     opMINUSMINUS = 308,
     opPLUSPLUS = 309,
     opSHL = 310,
     opSHR = 311,
     opPLASN = 312,
     opMIASN = 313,
     opMLASN = 314,
     opDVASN = 315,
     opMODASN = 316,
     opANDASN = 317,
     opXORASN = 318,
     opORASN = 319,
     opSLASN = 320,
     opSRASN = 321,
     opEQ = 322,
     opNE = 323,
     opGE = 324,
     opLE = 325,
     opAND = 326,
     opOR = 327,
     opCOLONCOLON = 328,
     opCATNEWLINE = 329,
     opCATSPACE = 330,
     opCATTAB = 331,
     END_TOK = 332
   };
#endif
/* Tokens.  */
#define START_TOK_SCRIPT 258
#define START_TOK_GDECL 259
#define rwSTATE 260
#define rwBETWEEN 261
#define rwOBJECT 262
#define rwCLASS 263
#define rwRECORD 264
#define rwDIRECTORY 265
#define rwCONNECTION 266
#define rwALIAS 267
#define rwFACET 268
#define rwIN 269
#define rwFROM 270
#define rwTO 271
#define rwVIA 272
#define rwTRY 273
#define rwCATCH 274
#define rwFINALLY 275
#define rwWHEN 276
#define rwFUNCTION 277
#define rwMETHOD 278
#define rwRETURN 279
#define rwWHILE 280
#define rwFOR 281
#define rwBREAK 282
#define rwCONTINUE 283
#define rwIF 284
#define rwELSE 285
#define rwSWITCH 286
#define rwCASE 287
#define rwDEFAULT 288
#define rwVAR 289
#define rwINTEGER 290
#define rwFLOAT 291
#define rwBOOL 292
#define rwSTRING 293
#define rwSHARED 294
#define rwPUBLIC 295
#define rwFILE 296
#define ILLEGAL_TOKEN 297
#define INT_CONSTANT 298
#define IDENTIFIER 299
#define STRING_CONSTANT 300
#define FLOAT_CONSTANT 301
#define PATH 302
#define STRING_FRAGMENT 303
#define STRING_END 304
#define INDENT 305
#define DEDENT 306
#define opARROW 307
#define opMINUSMINUS 308
#define opPLUSPLUS 309
#define opSHL 310
#define opSHR 311
#define opPLASN 312
#define opMIASN 313
#define opMLASN 314
#define opDVASN 315
#define opMODASN 316
#define opANDASN 317
#define opXORASN 318
#define opORASN 319
#define opSLASN 320
#define opSRASN 321
#define opEQ 322
#define opNE 323
#define opGE 324
#define opLE 325
#define opAND 326
#define opOR 327
#define opCOLONCOLON 328
#define opCATNEWLINE 329
#define opCATSPACE 330
#define opCATTAB 331
#define END_TOK 332




/* Copy the first part of user declarations.  */
#line 11 "parser.bison"

//%debug

// The kt programming language.  Parser grammar © 2012 Mark Frohnmayer.  The use of this code is governed by its license.  See /license/info.txt in the source distribution for the full license agreement.

#include "kt.h"

#define YYSTYPE parse_node_ptr
#define KT
class kt_lexer;
void kt_error (struct YYLTYPE *loc, kt_lexer *lexer, parse_result *result, const char *format, ...);
#ifndef YYDEBUG
//#define YYDEBUG 1
#endif

#define YYSSIZE 350

#define nil 0
#undef YY_ARGS
#define YY_ARGS(x)	x
#line 124 "parser.bison"

extern int kt_lex (YYSTYPE *lvalp, YYLTYPE *llocp, kt_lexer *lexer);
#define node(name) result->add_node(#name)
#define field(obj, fieldName, value) obj->set_property(result, #fieldName, value)
#define integer(num) result->add_int(num)
#define boolean(num) result->add_int(num)
#define string(str) result->add_string(str, strlen(str))
#define list(firstElem) result->add_list(firstElem)
#define empty_list() result->add_list(0)
#define append(list, elem) list->append(elem)
#define root(rootNode) result->root = rootNode
#define integer_value(node) node->int_data


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 315 "parser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  24
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   744

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  106
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  91
/* YYNRULES -- Number of rules.  */
#define YYNRULES  221
/* YYNRULES -- Number of states.  */
#define YYNSTATES  451

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   335

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    72,     2,     2,     2,    62,    61,     2,
      63,    64,    54,    52,    65,    53,    59,    55,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    66,    67,
      56,    58,    57,   105,    73,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   102,     2,   103,    70,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    68,    60,    69,    71,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   104
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    45,    46,
      48,    49,    53,    54,    56,    59,    64,    65,    68,    74,
      76,    78,    79,    82,    84,    88,    89,    91,    93,    97,
     104,   105,   108,   112,   121,   128,   138,   147,   152,   160,
     166,   173,   175,   178,   181,   183,   186,   190,   193,   199,
     200,   203,   210,   219,   225,   233,   234,   236,   237,   240,
     245,   258,   269,   279,   283,   288,   294,   295,   299,   300,
     302,   304,   313,   314,   317,   318,   322,   323,   325,   327,
     330,   332,   334,   336,   338,   340,   342,   344,   347,   350,
     353,   357,   360,   364,   372,   374,   377,   378,   382,   385,
     387,   390,   392,   394,   396,   400,   405,   413,   418,   425,
     434,   447,   449,   451,   453,   455,   457,   459,   463,   465,
     467,   473,   478,   481,   485,   490,   494,   495,   497,   499,
     501,   504,   507,   509,   514,   518,   523,   527,   530,   533,
     535,   539,   541,   544,   547,   550,   553,   556,   558,   562,
     566,   570,   572,   576,   580,   582,   586,   590,   592,   596,
     600,   604,   608,   610,   614,   618,   620,   624,   626,   630,
     632,   636,   638,   642,   644,   648,   650,   654,   658,   662,
     666,   668,   670,   672,   678,   680,   684,   688,   692,   694,
     696,   698,   700,   702,   704,   706,   708,   710,   712,   714,
     716,   719,   723,   728,   731,   735,   740,   742,   746,   748,
     752,   756
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     107,     0,    -1,   108,    -1,   109,    -1,   108,   109,    -1,
     110,    -1,   128,    -1,   129,    -1,   130,    -1,   135,    -1,
     139,    -1,   140,    -1,   141,    -1,   111,    -1,   124,    -1,
     125,    -1,   126,    -1,   112,     8,    44,   113,   116,   118,
     120,   145,   123,   144,    -1,    -1,    40,    -1,    -1,    63,
     114,    64,    -1,    -1,   115,    -1,    44,   134,    -1,   115,
      65,    44,   134,    -1,    -1,    66,   117,    -1,    66,   117,
      63,   193,    64,    -1,    44,    -1,    47,    -1,    -1,    14,
     119,    -1,    44,    -1,   119,    65,    44,    -1,    -1,   121,
      -1,   122,    -1,   121,    65,   122,    -1,    15,    44,    16,
      44,    17,   117,    -1,    -1,    50,    51,    -1,    50,   108,
      51,    -1,   112,     7,    44,   116,   118,   145,   123,   144,
      -1,    44,   116,   118,   145,   123,   144,    -1,   112,     9,
      44,   113,   116,   118,   145,   123,   144,    -1,   112,    11,
      44,   116,   127,   145,   123,   144,    -1,     6,    44,    65,
      44,    -1,   112,     5,    44,   120,   145,   123,   144,    -1,
      13,    44,   145,   123,   144,    -1,   112,   100,    44,   131,
     145,   144,    -1,   117,    -1,   131,   101,    -1,   131,   132,
      -1,   133,    -1,   102,   103,    -1,   102,   189,   103,    -1,
      68,    69,    -1,    68,   131,    65,   131,    69,    -1,    -1,
      66,   131,    -1,   112,   137,    34,    44,   138,   145,    -1,
     112,   137,    34,    44,    66,   131,   138,   145,    -1,   137,
      34,    44,   138,   145,    -1,   137,    34,    44,    66,   131,
     138,   145,    -1,    -1,    39,    -1,    -1,    58,   189,    -1,
      44,    58,   189,   145,    -1,   112,   137,    22,    44,    63,
     114,    64,   147,   118,   145,   148,   144,    -1,   112,   137,
      23,    44,   142,   147,   118,   145,   148,   144,    -1,   112,
     137,    23,    44,   147,   118,   145,   148,   144,    -1,    66,
     143,    44,    -1,   142,    66,   143,    44,    -1,   142,    44,
      66,   143,    44,    -1,    -1,    63,   131,    64,    -1,    -1,
      99,    -1,    99,    -1,    22,    44,    63,   114,    64,   147,
     145,   148,    -1,    -1,    74,   131,    -1,    -1,    50,   149,
      51,    -1,    -1,   150,    -1,   151,    -1,   150,   151,    -1,
     160,    -1,   161,    -1,   162,    -1,   163,    -1,   153,    -1,
     146,    -1,   136,    -1,    27,   145,    -1,    28,   145,    -1,
      24,   145,    -1,    24,   189,   145,    -1,   190,   145,    -1,
      50,   149,    51,    -1,    31,   189,   145,    50,   154,   155,
      51,    -1,   156,    -1,   154,   156,    -1,    -1,    33,   145,
     152,    -1,   157,   152,    -1,   159,    -1,   157,   159,    -1,
      43,    -1,    46,    -1,   167,    -1,    32,   158,   145,    -1,
      29,   189,   145,   152,    -1,    29,   189,   145,   152,    30,
     145,   152,    -1,    25,   189,   145,   152,    -1,   104,   145,
     152,    25,   189,   145,    -1,    26,   189,    67,   189,    67,
     189,   145,   152,    -1,    26,    34,    44,   134,    58,   189,
      67,   189,    67,   189,   145,   152,    -1,    44,    -1,    47,
      -1,    43,    -1,   165,    -1,    46,    -1,   167,    -1,    63,
     189,    64,    -1,   191,    -1,   192,    -1,   102,   189,    44,
     166,   103,    -1,   102,   189,    44,   103,    -1,    66,   189,
      -1,   166,    66,   189,    -1,   166,    44,    66,   189,    -1,
      48,   168,    49,    -1,    -1,   169,    -1,    48,    -1,    44,
      -1,   169,    48,    -1,   169,    44,    -1,   164,    -1,   170,
     102,   189,   103,    -1,   170,    63,    64,    -1,   170,    63,
     171,    64,    -1,   170,    59,    44,    -1,   170,    76,    -1,
     170,    75,    -1,   186,    -1,   171,    65,   186,    -1,   170,
      -1,    76,   172,    -1,    75,   172,    -1,    53,   172,    -1,
      72,   172,    -1,    71,   172,    -1,   172,    -1,   173,    54,
     172,    -1,   173,    55,   172,    -1,   173,    62,   172,    -1,
     173,    -1,   174,    52,   173,    -1,   174,    53,   173,    -1,
     174,    -1,   175,    77,   174,    -1,   175,    78,   174,    -1,
     175,    -1,   176,    56,   175,    -1,   176,    57,   175,    -1,
     176,    92,   175,    -1,   176,    91,   175,    -1,   176,    -1,
     177,    89,   176,    -1,   177,    90,   176,    -1,   177,    -1,
     178,    61,   177,    -1,   178,    -1,   179,    70,   178,    -1,
     179,    -1,   180,    60,   179,    -1,   180,    -1,   181,    93,
     180,    -1,   181,    -1,   182,    94,   181,    -1,   182,    -1,
     183,    73,   182,    -1,   183,    96,   182,    -1,   183,    97,
     182,    -1,   183,    98,   182,    -1,   183,    -1,   196,    -1,
     184,    -1,   184,   105,   189,    66,   185,    -1,   185,    -1,
     172,    58,   186,    -1,   172,   187,   186,    -1,   172,   188,
     186,    -1,    81,    -1,    82,    -1,    79,    -1,    80,    -1,
      83,    -1,    87,    -1,    88,    -1,    84,    -1,    85,    -1,
      86,    -1,   186,    -1,   189,    -1,   102,   103,    -1,   102,
     193,   103,    -1,   102,   193,    65,   103,    -1,    68,    69,
      -1,    68,   194,    69,    -1,    68,   194,    65,    69,    -1,
     189,    -1,   193,    65,   189,    -1,   195,    -1,   194,    65,
     195,    -1,   189,    66,   189,    -1,    22,    63,   114,    64,
     147,    63,   189,    64,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   141,   141,   147,   149,   154,   155,   156,   157,   158,
     159,   160,   161,   165,   166,   167,   168,   173,   188,   189,
     195,   196,   202,   203,   207,   215,   229,   234,   240,   249,
     251,   257,   258,   263,   265,   271,   272,   276,   278,   283,
     294,   295,   297,   303,   312,   325,   340,   353,   364,   376,
     386,   396,   401,   406,   412,   416,   418,   423,   424,   429,
     430,   436,   445,   457,   465,   477,   478,   484,   485,   491,
     501,   515,   526,   540,   549,   559,   573,   574,   578,   580,
     584,   588,   599,   600,   610,   611,   621,   622,   626,   628,
     633,   634,   635,   636,   637,   638,   639,   640,   642,   644,
     649,   654,   659,   664,   674,   676,   682,   683,   688,   696,
     698,   703,   708,   713,   720,   728,   734,   744,   753,   761,
     769,   792,   797,   802,   807,   808,   813,   815,   817,   818,
     822,   829,   839,   847,   856,   868,   889,   890,   895,   899,
     904,   913,   925,   926,   932,   938,   944,   950,   956,   965,
     967,   972,   973,   979,   985,   990,   995,  1003,  1004,  1011,
    1018,  1028,  1029,  1036,  1045,  1046,  1053,  1063,  1064,  1071,
    1078,  1085,  1095,  1096,  1103,  1113,  1114,  1124,  1125,  1135,
    1136,  1146,  1147,  1157,  1158,  1168,  1169,  1176,  1183,  1190,
    1200,  1201,  1205,  1206,  1216,  1217,  1223,  1230,  1240,  1242,
    1244,  1246,  1251,  1253,  1255,  1257,  1259,  1261,  1266,  1270,
    1278,  1283,  1288,  1296,  1298,  1303,  1311,  1313,  1318,  1320,
    1325,  1334
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "START_TOK_SCRIPT", "START_TOK_GDECL",
  "\"state\"", "\"between\"", "\"object\"", "\"class\"", "\"record\"",
  "\"directory\"", "\"connection\"", "\"alias\"", "\"facet\"", "\"in\"",
  "\"from\"", "\"to\"", "\"via\"", "\"try\"", "\"catch\"", "\"finally\"",
  "\"when\"", "\"function\"", "\"method\"", "\"return\"", "\"while\"",
  "\"for\"", "\"break\"", "\"continue\"", "\"if\"", "\"else\"",
  "\"switch\"", "\"case\"", "\"default\"", "\"var\"", "\"integer\"",
  "\"float\"", "\"boolean\"", "\"string\"", "\"shared\"", "\"public\"",
  "\"file\"", "ILLEGAL_TOKEN", "INT_CONSTANT", "IDENTIFIER",
  "STRING_CONSTANT", "FLOAT_CONSTANT", "PATH", "STRING_FRAGMENT",
  "STRING_END", "INDENT", "DEDENT", "'+'", "'-'", "'*'", "'/'", "'<'",
  "'>'", "'='", "'.'", "'|'", "'&'", "'%'", "'('", "')'", "','", "':'",
  "';'", "'{'", "'}'", "'^'", "'~'", "'!'", "'@'", "\"->\"", "\"--\"",
  "\"++\"", "\"<<\"", "\">>\"", "\"+=\"", "\"-=\"", "\"*=\"", "\"/=\"",
  "\"%=\"", "\"&=\"", "\"^=\"", "\"|=\"", "\"<<=\"", "\">>=\"", "\"==\"",
  "\"!=\"", "\">=\"", "\"<=\"", "\"&&\"", "\"||\"", "\"::\"", "\"NL\"",
  "\"SPC\"", "\"TAB\"", "END_TOK", "\"type\"", "\"&\"", "'['", "']'",
  "\"do\"", "'?'", "$accept", "start", "declaration_list", "declaration",
  "compound_declaration", "class_declaration", "is_public",
  "declaration_parameters", "optional_parameter_list", "parameter_list",
  "parent_specifier", "locator", "facet_list_specifier", "facet_name_list",
  "transmission_list_specifier", "transmission_list",
  "transmission_specifier", "compound_body", "object_declaration",
  "record_declaration", "connection_declaration", "between_specifier",
  "state_declaration", "facet_declaration", "type_declaration",
  "type_specifier", "array_specifier", "map_specifier",
  "optional_type_specifier", "slot_declaration",
  "variable_declaration_statement", "is_shared",
  "optional_assignment_expression", "slot_assignment_declaration",
  "function_declaration", "method_declaration", "selector_decl_args",
  "optional_selector_type_spec", "optional_end_token", "end_token",
  "function_declaration_statement", "optional_return_type",
  "function_body", "statement_list", "non_empty_statement_list",
  "statement", "statement_block", "switch_statement", "switch_list",
  "optional_default", "switch_element", "switch_label_list",
  "constant_atom", "switch_label", "if_statement", "while_statement",
  "do_while_statement", "for_statement", "primary_expression",
  "method_call", "selector_args", "fragmented_string",
  "fragment_or_ident_or_empty_list", "fragment_or_ident_list",
  "postfix_expression", "argument_expression_list", "unary_expression",
  "multiplicative_expression", "additive_expression", "shift_expression",
  "relational_expression", "equality_expression", "and_expression",
  "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression", "strcat_expression",
  "strcat_or_func_expression", "conditional_expression",
  "assignment_expression", "float_assignment_operator",
  "int_assignment_operator", "expression", "expression_statement",
  "array_expression", "map_expression", "expression_list", "map_pair_list",
  "map_pair", "function_expression", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,    43,    45,    42,    47,    60,    62,    61,    46,
     124,    38,    37,    40,    41,    44,    58,    59,   123,   125,
      94,   126,    33,    64,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,    91,    93,   335,    63
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   106,   107,   108,   108,   109,   109,   109,   109,   109,
     109,   109,   109,   110,   110,   110,   110,   111,   112,   112,
     113,   113,   114,   114,   115,   115,   116,   116,   116,   117,
     117,   118,   118,   119,   119,   120,   120,   121,   121,   122,
     123,   123,   123,   124,   124,   125,   126,   127,   128,   129,
     130,   131,   131,   131,   131,   132,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   137,   137,   138,   138,   139,
     140,   141,   141,   142,   142,   142,   143,   143,   144,   144,
     145,   146,   147,   147,   148,   148,   149,   149,   150,   150,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   152,   153,   154,   154,   155,   155,   156,   157,
     157,   158,   158,   158,   159,   160,   160,   161,   162,   163,
     163,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     165,   165,   166,   166,   166,   167,   168,   168,   169,   169,
     169,   169,   170,   170,   170,   170,   170,   170,   170,   171,
     171,   172,   172,   172,   172,   172,   172,   173,   173,   173,
     173,   174,   174,   174,   175,   175,   175,   176,   176,   176,
     176,   176,   177,   177,   177,   178,   178,   179,   179,   180,
     180,   181,   181,   182,   182,   183,   183,   183,   183,   183,
     184,   184,   185,   185,   186,   186,   186,   186,   187,   187,
     187,   187,   188,   188,   188,   188,   188,   188,   189,   190,
     191,   191,   191,   192,   192,   192,   193,   193,   194,   194,
     195,   196
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,    10,     0,     1,
       0,     3,     0,     1,     2,     4,     0,     2,     5,     1,
       1,     0,     2,     1,     3,     0,     1,     1,     3,     6,
       0,     2,     3,     8,     6,     9,     8,     4,     7,     5,
       6,     1,     2,     2,     1,     2,     3,     2,     5,     0,
       2,     6,     8,     5,     7,     0,     1,     0,     2,     4,
      12,    10,     9,     3,     4,     5,     0,     3,     0,     1,
       1,     8,     0,     2,     0,     3,     0,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     2,
       3,     2,     3,     7,     1,     2,     0,     3,     2,     1,
       2,     1,     1,     1,     3,     4,     7,     4,     6,     8,
      12,     1,     1,     1,     1,     1,     1,     3,     1,     1,
       5,     4,     2,     3,     4,     3,     0,     1,     1,     1,
       2,     2,     1,     4,     3,     4,     3,     2,     2,     1,
       3,     1,     2,     2,     2,     2,     2,     1,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     3,     3,     3,
       1,     1,     1,     5,     1,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     4,     2,     3,     4,     1,     3,     1,     3,
       3,     8
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      18,     0,    19,    26,     0,    18,     3,     5,    13,    65,
      14,    15,    16,     6,     7,     8,     9,    10,    11,    12,
       0,     0,     0,    31,     1,     4,     0,     0,     0,     0,
       0,    66,     0,     0,    80,    40,     0,   123,   121,   125,
     122,   136,     0,     0,     0,     0,     0,     0,     0,     0,
     142,   124,   126,   151,   157,   161,   164,   167,   172,   175,
     177,   179,   181,   183,   185,   190,   192,   194,   208,     0,
     128,   129,   191,    29,    30,    27,     0,     0,    35,    26,
      20,    20,    26,     0,     0,     0,     0,    18,    78,    22,
     139,   138,     0,   137,   154,     0,   213,     0,     0,   218,
     156,   155,   153,   152,   210,   216,     0,     0,     0,   148,
     147,     0,     0,   200,   201,   198,   199,   202,   205,   206,
     207,   203,   204,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    69,     0,
      33,    32,    40,     0,     0,    36,    37,    31,    22,    26,
      26,     0,     0,    51,     0,    54,     0,    82,    67,    41,
      18,    79,    49,    59,     0,    23,   135,   141,   140,   127,
       0,     0,   214,     0,     0,   211,   146,   144,     0,   149,
       0,   195,   196,   197,   158,   159,   160,   157,   162,   163,
     165,   166,   168,   169,   171,   170,   173,   174,   176,   178,
     180,   182,   184,   186,   187,   188,   189,     0,   216,     0,
       0,    78,     0,    40,     0,     0,     0,    31,    31,     0,
       0,    57,     0,    52,     0,    53,    78,    22,    76,     0,
      82,    31,     0,     0,     0,    42,     0,    24,    82,     0,
     220,   215,   219,     0,   131,     0,   212,   217,   145,     0,
     143,     0,    28,     0,    34,    44,     0,    78,    38,    40,
      21,    35,     0,     0,    40,     0,    55,     0,    50,     0,
       0,     0,    83,     0,    76,    31,     0,    68,    67,    61,
      60,     0,    59,   132,     0,     0,   130,   150,   193,     0,
      48,    78,     0,    40,     0,    78,     0,    56,    82,     0,
      73,    76,     0,     0,    84,     0,     0,    25,     0,   133,
       0,    43,    40,    78,    47,    46,    58,    31,    77,     0,
      74,    84,    65,    78,    62,     0,   134,    39,    78,    45,
       0,    75,    78,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    96,     0,    95,     0,    65,    88,    94,    90,
      91,    92,    93,   209,     0,    72,   221,    17,    84,    71,
       0,    99,     0,     0,     0,     0,    97,    98,     0,     0,
       0,     0,    85,    89,   101,    78,    22,   100,     0,    59,
       0,     0,     0,    65,     0,    67,    70,     0,   117,     0,
       0,   115,     0,     0,     0,     0,     0,    82,     0,     0,
       0,     0,   106,   104,     0,   109,   102,     0,    67,    63,
       0,     0,     0,     0,   111,   112,     0,   113,     0,     0,
     105,   108,   110,   118,     0,    84,     0,     0,   116,   114,
       0,   103,    64,    81,     0,   119,   107,     0,     0,     0,
     120
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     6,     7,     8,     9,   159,   174,   175,
      23,   163,    77,   151,   154,   155,   156,    88,    10,    11,
      12,   230,    13,    14,    15,   164,   235,   165,   247,    16,
     352,   353,   244,    17,    18,    19,   240,   281,   172,    35,
     354,   241,   333,   355,   356,   357,   394,   358,   412,   429,
     413,   414,   426,   415,   359,   360,   361,   362,    50,    51,
     255,    52,    92,    93,    53,   188,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,   123,   124,   363,   364,    70,    71,   106,    98,    99,
      72
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -350
static const yytype_int16 yypact[] =
{
     148,   -19,  -350,    57,    27,    13,  -350,  -350,  -350,    53,
    -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,
     -31,   631,   209,    59,  -350,  -350,    67,    75,   124,   129,
     140,  -350,   146,   180,  -350,   147,   143,  -350,  -350,  -350,
    -350,    -8,   642,   631,   455,   642,   642,   642,   642,   152,
    -350,  -350,  -350,   107,   643,   175,    72,   133,   -25,   150,
     155,   161,   173,   132,   141,     6,   154,  -350,  -350,   -31,
    -350,  -350,  -350,  -350,  -350,   189,   222,   -31,   253,   207,
     212,   212,   207,    -9,   234,   237,   240,    31,   204,   268,
    -350,  -350,   264,    -7,  -350,   250,  -350,   249,   122,  -350,
    -350,  -350,  -350,  -350,  -350,   274,   -49,   275,   491,  -350,
    -350,   631,   631,  -350,  -350,  -350,  -350,  -350,  -350,  -350,
    -350,  -350,  -350,   631,   631,   642,   642,   642,   642,   642,
     642,   642,   642,   642,   642,   642,   642,   642,   642,   642,
     642,   642,   642,   642,   642,   642,   642,   631,  -350,   631,
    -350,   255,   147,   277,   -31,   257,  -350,    59,   268,   207,
     207,   320,   111,  -350,   142,  -350,   267,    43,   120,  -350,
     168,  -350,  -350,   261,   269,   266,  -350,  -350,  -350,  -350,
     631,   526,  -350,   -58,   214,  -350,  -350,  -350,   199,  -350,
     229,  -350,  -350,  -350,  -350,  -350,  -350,  -350,   175,   175,
      72,    72,   133,   133,   133,   133,   -25,   -25,   150,   155,
     161,   173,   132,   141,   141,   141,   141,   270,  -350,   206,
     290,   204,   319,   147,   253,   -31,   273,    59,    59,   295,
     -31,  -350,   -46,  -350,   419,  -350,   204,   268,   278,    -9,
      19,    59,   631,    -9,   -31,  -350,    -9,  -350,   271,   296,
    -350,  -350,  -350,   631,  -350,   -33,  -350,  -350,  -350,   631,
    -350,   631,  -350,   631,  -350,  -350,   299,   204,  -350,   147,
    -350,   253,   -31,   279,   147,    -9,  -350,   243,  -350,   283,
      -9,   304,   186,   284,   278,    59,   -31,  -350,   -24,  -350,
     186,   288,   261,  -350,   287,   631,  -350,  -350,  -350,   338,
    -350,   204,   -31,   147,   312,   204,    12,  -350,   271,     5,
    -350,   278,   313,   -31,   309,   -31,   631,  -350,   631,  -350,
     209,  -350,   147,   204,  -350,  -350,  -350,    59,  -350,   317,
    -350,   309,   346,   204,  -350,   298,  -350,  -350,   204,  -350,
     -31,  -350,   204,    55,   561,   631,   596,   -31,   -31,   631,
     631,   -31,  -350,   329,  -350,   314,   408,  -350,  -350,  -350,
    -350,  -350,  -350,  -350,   -31,  -350,  -350,  -350,   309,  -350,
     301,  -350,   -31,   -31,   323,   311,  -350,  -350,   -31,   -31,
     326,   335,  -350,  -350,  -350,   204,   268,  -350,   326,   261,
     631,   326,   331,   346,   357,   135,  -350,   322,  -350,   333,
     328,   354,   364,   347,   631,    -9,   -31,   271,   631,   631,
     -31,   121,   259,  -350,    51,  -350,  -350,   -31,   -24,  -350,
     -31,   334,   -31,   326,  -350,  -350,   -31,  -350,   -31,   349,
    -350,  -350,  -350,  -350,   -31,   309,   631,   326,  -350,  -350,
     326,  -350,  -350,  -350,   336,  -350,  -350,   631,   -31,   326,
    -350
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -350,  -350,   315,     2,  -350,  -350,  -350,   324,  -148,  -350,
     -62,   -20,  -133,  -350,   136,  -350,   182,  -147,  -350,  -350,
    -350,  -350,  -350,  -350,  -350,  -159,  -350,  -350,  -277,  -350,
    -350,   395,  -241,  -350,  -350,  -350,  -350,  -263,  -116,   -68,
    -350,  -236,  -325,    15,  -350,    54,  -349,  -350,  -350,  -350,
      -1,  -350,  -350,    -2,  -350,  -350,  -350,  -350,  -350,  -350,
    -350,     8,  -350,  -350,  -350,  -350,     4,   165,   171,   116,
     162,   282,   285,   276,   272,   281,   163,  -350,  -350,   164,
     -94,  -350,  -350,   -21,  -350,  -350,  -350,   280,  -350,   246,
    -350
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -88
static const yytype_int16 yytable[] =
{
      69,   148,    75,   232,   285,   221,   342,    25,   253,   152,
     226,   294,   291,    -2,   189,   317,   184,   157,   191,   275,
     161,   312,    95,    97,   225,    20,     1,    24,   105,   192,
     193,   132,   133,   295,   242,    73,    90,   177,    74,   398,
      91,   178,   401,   385,     1,   254,    94,   315,   329,   100,
     101,   102,   103,     2,   185,   233,   234,     3,    26,   162,
      27,    28,    29,   283,    30,   431,   134,   135,    34,   328,
     296,     2,   327,    76,   438,     3,   267,   233,   234,   143,
     282,   326,   169,   411,   288,   284,   223,   290,   445,   279,
     190,   446,    31,   239,   271,   272,   236,   227,   228,   370,
     450,   393,   144,   145,   146,   265,   233,   234,   286,   238,
     443,    78,   399,   233,   234,    21,   306,   239,    89,    79,
     278,   309,   301,    22,   128,   129,   217,   305,   218,   194,
     195,   196,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   300,   313,    32,   406,    73,   323,   269,    74,   250,
      97,     1,   274,   257,   424,   297,   107,   425,    80,    41,
     108,   420,    25,    81,    36,   338,   289,   434,   242,   162,
     231,     1,   109,   110,    82,   321,   243,   181,     2,   325,
      83,   182,     3,   242,   340,    37,    38,    87,    39,    40,
      41,   405,    84,    85,   303,    42,    89,   339,     2,   111,
     130,   131,     3,   277,    86,    43,   138,   365,   314,   245,
      44,   287,   367,    45,    46,   141,   369,    47,    48,   125,
     126,   139,   293,   140,   322,   142,    36,   127,   397,   136,
     137,    34,   257,   233,   234,   331,   418,   334,   202,   203,
     204,   205,   149,    73,    49,   104,    74,    37,    38,   147,
      39,    40,    41,   258,   259,   197,   150,    42,   153,   396,
     262,   263,   368,    22,   319,   158,   371,    43,   166,   376,
     377,   167,    44,   380,   168,    45,    46,   233,   234,    47,
      48,   411,   428,   198,   199,   335,   384,   336,   206,   207,
     337,   200,   201,   171,   387,   388,   213,   214,   215,   216,
     391,   392,   173,   176,   179,   180,    49,   256,   183,   186,
     220,   222,   224,   372,   373,   375,   229,   246,   378,   379,
     237,   249,   260,   248,   264,   266,   261,   270,   419,   273,
     292,   280,   423,   299,   304,   239,   307,   308,   310,   433,
     311,   316,   435,   318,   437,   320,   324,   330,   439,   332,
     440,   341,   366,   381,   386,   382,   442,   389,   343,   400,
     344,   345,   346,   347,   348,   349,   393,   350,   390,   395,
     449,   402,   404,   417,   410,    31,   407,   421,   422,    37,
      38,   408,    39,    40,    41,   409,   411,   -86,   416,    42,
     441,   436,   170,   447,    33,   160,   268,   302,   403,    43,
     383,   430,   432,   211,    44,   444,   210,    45,    46,   427,
     208,    47,    48,   212,   209,   298,   448,   252,     0,   219,
     343,     0,   344,   345,   346,   347,   348,   349,     0,   350,
       0,    36,     0,     0,     0,     0,     0,    31,    49,     0,
     351,    37,    38,     0,    39,    40,    41,     0,     0,   -87,
       0,    42,    37,    38,     0,    39,    40,    41,     0,     0,
       0,    43,    42,     0,     0,     0,    44,    36,     0,    45,
      46,     0,    43,    47,    48,     0,     0,    44,     0,     0,
      45,    46,     0,     0,    47,    48,     0,     0,    37,    38,
       0,    39,    40,    41,     0,     0,     0,     0,    42,     0,
      49,     0,   351,    36,     0,     0,     0,     0,    43,     0,
       0,    49,   276,    44,    96,     0,    45,    46,     0,     0,
      47,    48,     0,     0,    37,    38,     0,    39,    40,    41,
       0,     0,     0,     0,    42,     0,     0,     0,    36,     0,
       0,     0,     0,     0,    43,   187,     0,    49,     0,    44,
       0,     0,    45,    46,     0,     0,    47,    48,     0,    37,
      38,     0,    39,    40,    41,     0,     0,     0,     0,    42,
       0,     0,     0,    36,     0,     0,     0,     0,     0,    43,
       0,     0,     0,    49,    44,   251,     0,    45,    46,     0,
       0,    47,    48,     0,    37,    38,     0,    39,    40,    41,
       0,     0,     0,     0,    42,     0,     0,     0,    36,     0,
       0,     0,     0,     0,    43,     0,     0,     0,    49,    44,
     374,     0,    45,    46,     0,     0,    47,    48,     0,    37,
      38,     0,    39,    40,    41,     0,     0,     0,     0,    42,
       0,     0,     0,    36,     0,     0,     0,     0,     0,    43,
      34,     0,     0,    49,    44,     0,     0,    45,    46,     0,
       0,    47,    48,     0,    37,    38,     0,    39,    40,    41,
       0,     0,     0,     0,    42,    37,    38,     0,    39,    40,
      41,     0,     0,     0,    43,    42,     0,     0,    49,    44,
       0,   112,    45,    46,     0,    43,    47,    48,     0,     0,
      44,     0,     0,    45,    46,     0,     0,    47,    48,     0,
       0,     0,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,     0,    49,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    49
};

static const yytype_int16 yycheck[] =
{
      21,    69,    22,   162,   240,   152,   331,     5,    66,    77,
     158,    44,   248,     0,   108,   292,    65,    79,   112,    65,
      82,   284,    43,    44,   157,    44,    13,     0,    49,   123,
     124,    56,    57,    66,    58,    44,    44,    44,    47,   388,
      48,    48,   391,   368,    13,   103,    42,   288,   311,    45,
      46,    47,    48,    40,   103,   101,   102,    44,     5,    68,
       7,     8,     9,    44,    11,   414,    91,    92,    99,    64,
     103,    40,   308,    14,   423,    44,   223,   101,   102,    73,
     239,    69,    51,    32,   243,    66,   154,   246,   437,   237,
     111,   440,    39,    74,   227,   228,   164,   159,   160,    44,
     449,    50,    96,    97,    98,   221,   101,   102,   241,    66,
     435,    44,   389,   101,   102,    58,   275,    74,    63,    44,
     236,   280,   269,    66,    52,    53,   147,   274,   149,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   267,   285,   100,   395,    44,   303,   225,    47,   180,
     181,    13,   230,   184,    43,   259,    59,    46,    44,    48,
      63,   407,   170,    44,    22,   322,   244,   418,    58,    68,
      69,    13,    75,    76,    44,   301,    66,    65,    40,   305,
      44,    69,    44,    58,   327,    43,    44,    50,    46,    47,
      48,    66,    22,    23,   272,    53,    63,   323,    40,   102,
      77,    78,    44,   234,    34,    63,    61,   333,   286,    51,
      68,   242,   338,    71,    72,    93,   342,    75,    76,    54,
      55,    70,   253,    60,   302,    94,    22,    62,   386,    89,
      90,    99,   263,   101,   102,   313,   405,   315,   132,   133,
     134,   135,    63,    44,   102,   103,    47,    43,    44,   105,
      46,    47,    48,    64,    65,   261,    44,    53,    15,   385,
      64,    65,   340,    66,   295,    63,   344,    63,    44,   347,
     348,    44,    68,   351,    44,    71,    72,   101,   102,    75,
      76,    32,    33,   128,   129,   316,   364,   318,   136,   137,
     320,   130,   131,    99,   372,   373,   143,   144,   145,   146,
     378,   379,    44,    49,    64,    66,   102,   103,    44,    44,
      65,    44,    65,   344,   345,   346,     6,    66,   349,   350,
      63,    65,   103,    64,    44,    16,    66,    64,   406,    44,
      44,    63,   410,    44,    65,    74,   103,    64,    44,   417,
      66,    63,   420,    66,   422,    17,    44,    44,   426,    50,
     428,    44,    64,    34,    63,    51,   434,    44,    22,   390,
      24,    25,    26,    27,    28,    29,    50,    31,    67,    44,
     448,    50,    25,   404,    30,    39,    64,   408,   409,    43,
      44,    58,    46,    47,    48,    67,    32,    51,    51,    53,
      51,    67,    87,    67,     9,    81,   224,   271,   393,    63,
     356,   412,   414,   141,    68,   436,   140,    71,    72,   411,
     138,    75,    76,   142,   139,   261,   447,   181,    -1,   149,
      22,    -1,    24,    25,    26,    27,    28,    29,    -1,    31,
      -1,    22,    -1,    -1,    -1,    -1,    -1,    39,   102,    -1,
     104,    43,    44,    -1,    46,    47,    48,    -1,    -1,    51,
      -1,    53,    43,    44,    -1,    46,    47,    48,    -1,    -1,
      -1,    63,    53,    -1,    -1,    -1,    68,    22,    -1,    71,
      72,    -1,    63,    75,    76,    -1,    -1,    68,    -1,    -1,
      71,    72,    -1,    -1,    75,    76,    -1,    -1,    43,    44,
      -1,    46,    47,    48,    -1,    -1,    -1,    -1,    53,    -1,
     102,    -1,   104,    22,    -1,    -1,    -1,    -1,    63,    -1,
      -1,   102,   103,    68,    69,    -1,    71,    72,    -1,    -1,
      75,    76,    -1,    -1,    43,    44,    -1,    46,    47,    48,
      -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    63,    64,    -1,   102,    -1,    68,
      -1,    -1,    71,    72,    -1,    -1,    75,    76,    -1,    43,
      44,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,    53,
      -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    63,
      -1,    -1,    -1,   102,    68,    69,    -1,    71,    72,    -1,
      -1,    75,    76,    -1,    43,    44,    -1,    46,    47,    48,
      -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,   102,    68,
      34,    -1,    71,    72,    -1,    -1,    75,    76,    -1,    43,
      44,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,    53,
      -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    63,
      99,    -1,    -1,   102,    68,    -1,    -1,    71,    72,    -1,
      -1,    75,    76,    -1,    43,    44,    -1,    46,    47,    48,
      -1,    -1,    -1,    -1,    53,    43,    44,    -1,    46,    47,
      48,    -1,    -1,    -1,    63,    53,    -1,    -1,   102,    68,
      -1,    58,    71,    72,    -1,    63,    75,    76,    -1,    -1,
      68,    -1,    -1,    71,    72,    -1,    -1,    75,    76,    -1,
      -1,    -1,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    -1,   102,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   102
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    13,    40,    44,   107,   108,   109,   110,   111,   112,
     124,   125,   126,   128,   129,   130,   135,   139,   140,   141,
      44,    58,    66,   116,     0,   109,     5,     7,     8,     9,
      11,    39,   100,   137,    99,   145,    22,    43,    44,    46,
      47,    48,    53,    63,    68,    71,    72,    75,    76,   102,
     164,   165,   167,   170,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   189,
     191,   192,   196,    44,    47,   117,    14,   118,    44,    44,
      44,    44,    44,    44,    22,    23,    34,    50,   123,    63,
      44,    48,   168,   169,   172,   189,    69,   189,   194,   195,
     172,   172,   172,   172,   103,   189,   193,    59,    63,    75,
      76,   102,    58,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,   187,   188,    54,    55,    62,    52,    53,
      77,    78,    56,    57,    91,    92,    89,    90,    61,    70,
      60,    93,    94,    73,    96,    97,    98,   105,   145,    63,
      44,   119,   145,    15,   120,   121,   122,   116,    63,   113,
     113,   116,    68,   117,   131,   133,    44,    44,    44,    51,
     108,    99,   144,    44,   114,   115,    49,    44,    48,    64,
      66,    65,    69,    44,    65,   103,    44,    64,   171,   186,
     189,   186,   186,   186,   172,   172,   172,   172,   173,   173,
     174,   174,   175,   175,   175,   175,   176,   176,   177,   178,
     179,   180,   181,   182,   182,   182,   182,   189,   189,   193,
      65,   123,    44,   145,    65,   118,   114,   116,   116,     6,
     127,    69,   131,   101,   102,   132,   145,    63,    66,    74,
     142,   147,    58,    66,   138,    51,    66,   134,    64,    65,
     189,    69,   195,    66,   103,   166,   103,   189,    64,    65,
     103,    66,    64,    65,    44,   144,    16,   123,   122,   145,
      64,   118,   118,    44,   145,    65,   103,   189,   144,   114,
      63,   143,   131,    44,    66,   147,   118,   189,   131,   145,
     131,   147,    44,   189,    44,    66,   103,   186,   185,    44,
     144,   123,   120,   145,    65,   123,   131,   103,    64,   131,
      44,    66,   143,   118,   145,   138,    63,   134,    66,   189,
      17,   144,   145,   123,    44,   144,    69,   147,    64,   143,
      44,   145,    50,   148,   145,   189,   189,   117,   123,   144,
     118,    44,   148,    22,    24,    25,    26,    27,    28,    29,
      31,   104,   136,   137,   146,   149,   150,   151,   153,   160,
     161,   162,   163,   189,   190,   144,    64,   144,   145,   144,
      44,   145,   189,   189,    34,   189,   145,   145,   189,   189,
     145,    34,    51,   151,   145,   148,    63,   145,   145,    44,
      67,   145,   145,    50,   152,    44,   144,   114,   152,   134,
     189,   152,    50,   149,    25,    66,   138,    64,    58,    67,
      30,    32,   154,   156,   157,   159,    51,   189,   131,   145,
     147,   189,   189,   145,    43,    46,   158,   167,    33,   155,
     156,   152,   159,   145,   138,   145,    67,   145,   152,   145,
     145,    51,   145,   148,   189,   152,   152,    67,   189,   145,
     152
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (&yylloc, lexer, result, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, lexer)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, lexer, result); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, kt_lexer *lexer, parse_result *result)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, lexer, result)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    kt_lexer *lexer;
    parse_result *result;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (lexer);
  YYUSE (result);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, kt_lexer *lexer, parse_result *result)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, lexer, result)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    kt_lexer *lexer;
    parse_result *result;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, lexer, result);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, kt_lexer *lexer, parse_result *result)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, lexer, result)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    kt_lexer *lexer;
    parse_result *result;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , lexer, result);
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, lexer, result); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
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

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, kt_lexer *lexer, parse_result *result)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, lexer, result)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    kt_lexer *lexer;
    parse_result *result;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (lexer);
  YYUSE (result);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (kt_lexer *lexer, parse_result *result);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (kt_lexer *lexer, parse_result *result)
#else
int
yyparse (lexer, result)
    kt_lexer *lexer;
    parse_result *result;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 142 "parser.bison"
    { root((yyval)); ;}
    break;

  case 3:
#line 148 "parser.bison"
    { (yyval) = list((yyvsp[(1) - (1)])); ;}
    break;

  case 4:
#line 150 "parser.bison"
    { (yyval) = (yyvsp[(1) - (2)]); append((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 17:
#line 174 "parser.bison"
    {
			(yyval) = node(class);
			field((yyval), name, (yyvsp[(3) - (10)]));
			field((yyval), decl_flags, (yyvsp[(1) - (10)]));
			field((yyval), parameter_list, (yyvsp[(4) - (10)]));
			field((yyval), parent_decl, (yyvsp[(5) - (10)]));
			field((yyval), facet_list, (yyvsp[(6) - (10)]));
			field((yyval), transmission_list, (yyvsp[(7) - (10)]));
			field((yyval), body, (yyvsp[(9) - (10)]));
		;}
    break;

  case 18:
#line 188 "parser.bison"
    { (yyval) = boolean(false); ;}
    break;

  case 19:
#line 190 "parser.bison"
    { (yyval) = boolean(true); ;}
    break;

  case 20:
#line 195 "parser.bison"
    { (yyval) = empty_list(); ;}
    break;

  case 21:
#line 197 "parser.bison"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 22:
#line 202 "parser.bison"
    { (yyval) = empty_list(); ;}
    break;

  case 24:
#line 208 "parser.bison"
    {
		YYSTYPE var;
		var = node(parameter);
		field(var, name, (yyvsp[(1) - (2)]));
		field(var, type_spec, (yyvsp[(2) - (2)]));
		(yyval) = list(var);
	;}
    break;

  case 25:
#line 216 "parser.bison"
    {
		YYSTYPE var;
		var = node(parameter);
		field(var, name, (yyvsp[(3) - (4)]));
		field(var, type_spec, (yyvsp[(4) - (4)]));
		(yyval) = (yyvsp[(1) - (4)]);
		append((yyvsp[(1) - (4)]), var);
	;}
    break;

  case 26:
#line 229 "parser.bison"
    {
		(yyval) = node(parent_specifier);
		field((yyval), name, nil);
		field((yyval), args, nil);
	;}
    break;

  case 27:
#line 235 "parser.bison"
    {
		(yyval) = node(parent_specifier);
		field((yyval), name, (yyvsp[(2) - (2)]));
		field((yyval), args, list(nil));
	;}
    break;

  case 28:
#line 241 "parser.bison"
    {
		(yyval) = node(parent_specifier);
		field((yyval), name, (yyvsp[(2) - (5)]));
		field((yyval), args, (yyvsp[(4) - (5)]));
	;}
    break;

  case 29:
#line 250 "parser.bison"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 30:
#line 252 "parser.bison"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 31:
#line 257 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 32:
#line 259 "parser.bison"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 33:
#line 264 "parser.bison"
    { (yyval) = list((yyvsp[(1) - (1)])); ;}
    break;

  case 34:
#line 266 "parser.bison"
    { (yyval) = (yyvsp[(1) - (3)]); append((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 35:
#line 271 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 37:
#line 277 "parser.bison"
    { (yyval) = list((yyvsp[(1) - (1)])); ;}
    break;

  case 38:
#line 279 "parser.bison"
    { (yyval) = (yyvsp[(1) - (3)]); append((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 39:
#line 284 "parser.bison"
    {
         (yyval) = node(transmission_specifier);
         field((yyval), from_facet, (yyvsp[(2) - (6)]));
         field((yyval), to_facet, (yyvsp[(4) - (6)]));
         field((yyval), via, (yyvsp[(6) - (6)]));
      ;}
    break;

  case 40:
#line 294 "parser.bison"
    { (yyval) = empty_list(); ;}
    break;

  case 41:
#line 296 "parser.bison"
    { (yyval) = empty_list(); ;}
    break;

  case 42:
#line 298 "parser.bison"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 43:
#line 304 "parser.bison"
    { 
			(yyval) = node(object);
			field((yyval), is_public, (yyvsp[(1) - (8)]));
			field((yyval), name, (yyvsp[(3) - (8)]));
			field((yyval), parent_decl, (yyvsp[(4) - (8)]));
			field((yyval), facet_list, (yyvsp[(5) - (8)]));
			field((yyval), body, (yyvsp[(7) - (8)]));
		;}
    break;

  case 44:
#line 313 "parser.bison"
    { 
			(yyval) = node(object);
			field((yyval), is_public, boolean(false));
			field((yyval), name, (yyvsp[(1) - (6)]));
			field((yyval), parent_decl, (yyvsp[(2) - (6)]));
			field((yyval), facet_list, (yyvsp[(3) - (6)]));
			field((yyval), body, (yyvsp[(5) - (6)]));
		;}
    break;

  case 45:
#line 327 "parser.bison"
    {
         (yyval) = node(record);
         field((yyval), is_public, (yyvsp[(1) - (9)]));
         field((yyval), name, (yyvsp[(3) - (9)]));
         field((yyval), parameter_list, (yyvsp[(4) - (9)]));
         field((yyval), parent_decl, (yyvsp[(5) - (9)]));
         field((yyval), facet_list, (yyvsp[(6) - (9)]));
         field((yyval), body, (yyvsp[(8) - (9)]));
      ;}
    break;

  case 46:
#line 341 "parser.bison"
    {
         (yyval) = node(connection);
         field((yyval), is_public, (yyvsp[(1) - (8)]));
         field((yyval), name, (yyvsp[(3) - (8)]));
         field((yyval), parent_decl, (yyvsp[(4) - (8)]));
         field((yyval), transmission_list, (yyvsp[(5) - (8)]));
         field((yyval), body, (yyvsp[(7) - (8)]));
      ;}
    break;

  case 47:
#line 354 "parser.bison"
    {
         (yyval) = node(transmission_specifier);
         field((yyval), from_facet, (yyvsp[(2) - (4)]));
         field((yyval), to_facet, (yyvsp[(4) - (4)]));
         (yyval) = list((yyval));
      ;}
    break;

  case 48:
#line 365 "parser.bison"
    {
			(yyval) = node(state);
			field((yyval), is_public, (yyvsp[(1) - (7)]));
			field((yyval), name, (yyvsp[(3) - (7)]));
			field((yyval), transmission_list, (yyvsp[(4) - (7)]));
			field((yyval), body, (yyvsp[(6) - (7)]));
		;}
    break;

  case 49:
#line 377 "parser.bison"
    {
			(yyval) = node(facet);
			field((yyval), name, (yyvsp[(2) - (5)]));
			field((yyval), body, (yyvsp[(4) - (5)]));
		;}
    break;

  case 50:
#line 387 "parser.bison"
    {
         (yyval) = node(type);
         field((yyval), is_public, (yyvsp[(1) - (6)]));
         field((yyval), name, (yyvsp[(3) - (6)]));
         field((yyval), type_spec, (yyvsp[(4) - (6)]));
      ;}
    break;

  case 51:
#line 397 "parser.bison"
    {
         (yyval) = node(locator_type_specifier);
         field((yyval), locator, (yyvsp[(1) - (1)]));
      ;}
    break;

  case 52:
#line 402 "parser.bison"
    {
         (yyval) = node(reference_type_specifier);
         field((yyval), base_type, (yyvsp[(1) - (2)]));
      ;}
    break;

  case 53:
#line 407 "parser.bison"
    {
         (yyval) = node(array_type_specifier);
         field((yyval), base_type, (yyvsp[(1) - (2)]));
         field((yyval), size_expr, (yyvsp[(2) - (2)]));
      ;}
    break;

  case 55:
#line 417 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 56:
#line 419 "parser.bison"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 59:
#line 429 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 60:
#line 431 "parser.bison"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 61:
#line 437 "parser.bison"
    {
         (yyval) = node(slot_declaration);
         field((yyval), is_public, (yyvsp[(1) - (6)]));
         field((yyval), is_shared, (yyvsp[(2) - (6)]));
         field((yyval), name, (yyvsp[(4) - (6)]));
         field((yyval), assign_expr, (yyvsp[(5) - (6)]));
         field((yyval), type_spec, nil);
      ;}
    break;

  case 62:
#line 446 "parser.bison"
    {
         (yyval) = node(slot_declaration);
         field((yyval), is_public, (yyvsp[(1) - (8)]));
         field((yyval), is_shared, (yyvsp[(2) - (8)]));
         field((yyval), name, (yyvsp[(4) - (8)]));
         field((yyval), type_spec, (yyvsp[(6) - (8)]));
         field((yyval), assign_expr, (yyvsp[(7) - (8)]));
      ;}
    break;

  case 63:
#line 458 "parser.bison"
    {
			(yyval) = node(variable_declaration_stmt);
			field((yyval), is_shared, (yyvsp[(1) - (5)]));
			field((yyval), name, (yyvsp[(3) - (5)]));
			field((yyval), assign_expr, (yyvsp[(4) - (5)]));
			field((yyval), type_spec, nil);
		;}
    break;

  case 64:
#line 466 "parser.bison"
    {
			(yyval) = node(variable_declaration_stmt);
			field((yyval), is_shared, (yyvsp[(1) - (7)]));
			field((yyval), name, (yyvsp[(3) - (7)]));
			field((yyval), type_spec, (yyvsp[(5) - (7)]));
			field((yyval), assign_expr, (yyvsp[(6) - (7)]));
		;}
    break;

  case 65:
#line 477 "parser.bison"
    { (yyval) = boolean(false); ;}
    break;

  case 66:
#line 479 "parser.bison"
    { (yyval) = boolean(true); ;}
    break;

  case 67:
#line 484 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 68:
#line 486 "parser.bison"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 69:
#line 492 "parser.bison"
    {
	     (yyval) = node(slot_assignment);
		 field((yyval), name, (yyvsp[(1) - (4)]));
		 field((yyval), assign_expr, (yyvsp[(3) - (4)]));
      ;}
    break;

  case 70:
#line 502 "parser.bison"
    {
			(yyval) = node(function);
			field((yyval), name, (yyvsp[(4) - (12)]));
			field((yyval), is_public, (yyvsp[(1) - (12)]));
			field((yyval), is_shared, (yyvsp[(2) - (12)]));
			field((yyval), parameter_list, (yyvsp[(6) - (12)]));
			field((yyval), return_type, (yyvsp[(8) - (12)]));
			field((yyval), facet_list, (yyvsp[(9) - (12)]));
			field((yyval), statements, (yyvsp[(11) - (12)]));
		;}
    break;

  case 71:
#line 516 "parser.bison"
    {
			(yyval) = node(function);
			field((yyval), primary_name, (yyvsp[(4) - (10)]));
			field((yyval), selector_decl_list, (yyvsp[(5) - (10)]));
			field((yyval), is_public, (yyvsp[(1) - (10)]));
			field((yyval), is_shared, (yyvsp[(2) - (10)]));
			field((yyval), return_type, (yyvsp[(6) - (10)]));
			field((yyval), facet_list, (yyvsp[(7) - (10)]));
			field((yyval), statements, (yyvsp[(9) - (10)]));
		;}
    break;

  case 72:
#line 527 "parser.bison"
    {
			(yyval) = node(function);
			field((yyval), primary_name, (yyvsp[(4) - (9)]));
			field((yyval), selector_decl_list, empty_list());
			field((yyval), is_public, (yyvsp[(1) - (9)]));
			field((yyval), is_shared, (yyvsp[(2) - (9)]));
			field((yyval), return_type, (yyvsp[(5) - (9)]));
			field((yyval), facet_list, (yyvsp[(6) - (9)]));
			field((yyval), statements, (yyvsp[(8) - (9)]));
		;}
    break;

  case 73:
#line 541 "parser.bison"
    {
			YYSTYPE pair;
			pair = node(selector_pair);
			field(pair, string, nil);
			field(pair, type_spec, (yyvsp[(2) - (3)]));
			field(pair, name, (yyvsp[(3) - (3)]));
			(yyval) = list(pair);
		;}
    break;

  case 74:
#line 550 "parser.bison"
    {
			YYSTYPE pair;
			pair = node(selector_pair);
			field(pair, string, nil);
			field(pair, type_spec, (yyvsp[(3) - (4)]));
			field(pair, name, (yyvsp[(4) - (4)]));
			append((yyvsp[(1) - (4)]), pair);
			(yyval) = (yyvsp[(1) - (4)]);
		;}
    break;

  case 75:
#line 560 "parser.bison"
    {
			YYSTYPE pair;
			pair = node(selector_pair);
			field(pair, string, (yyvsp[(2) - (5)]));
			field(pair, type_spec, (yyvsp[(4) - (5)]));
			field(pair, name, (yyvsp[(5) - (5)]));
			append((yyvsp[(1) - (5)]), pair);
			(yyval) = (yyvsp[(1) - (5)]);
		;}
    break;

  case 76:
#line 573 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 77:
#line 575 "parser.bison"
    { (yyval) = (yyvsp[(1) - (3)]); ;}
    break;

  case 81:
#line 589 "parser.bison"
    {
         (yyval) = node(function_declaration_stmt);
         field((yyval), name, (yyvsp[(2) - (8)]));
         field((yyval), parameter_list, (yyvsp[(4) - (8)]));
         field((yyval), return_type, (yyvsp[(6) - (8)]));
         field((yyval), statements, (yyvsp[(8) - (8)]));
      ;}
    break;

  case 82:
#line 599 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 83:
#line 601 "parser.bison"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 84:
#line 610 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 85:
#line 612 "parser.bison"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 86:
#line 621 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 88:
#line 627 "parser.bison"
    { (yyval) = list((yyvsp[(1) - (1)])); ;}
    break;

  case 89:
#line 629 "parser.bison"
    { (yyval) = (yyvsp[(1) - (2)]); append((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 97:
#line 641 "parser.bison"
    { (yyval) = node(break_stmt); ;}
    break;

  case 98:
#line 643 "parser.bison"
    { (yyval) = node(continue_stmt); ;}
    break;

  case 99:
#line 645 "parser.bison"
    { 
			(yyval) = node(return_stmt);
			field((yyval), return_expression_list, empty_list());
		;}
    break;

  case 100:
#line 650 "parser.bison"
    {
			(yyval) = node(return_stmt);
			field((yyval), return_expression, (yyvsp[(2) - (3)]));
		;}
    break;

  case 101:
#line 655 "parser.bison"
    { (yyval) = (yyvsp[(1) - (2)]); ;}
    break;

  case 102:
#line 660 "parser.bison"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 103:
#line 665 "parser.bison"
    {
         (yyval) = node(switch_stmt);
         field((yyval), test_expression, (yyvsp[(2) - (7)]));
         field((yyval), element_list, (yyvsp[(5) - (7)]));
		 field((yyval), default_block, (yyvsp[(6) - (7)]));
      ;}
    break;

  case 104:
#line 675 "parser.bison"
    { (yyval) = list((yyvsp[(1) - (1)])); ;}
    break;

  case 105:
#line 677 "parser.bison"
    { (yyval) = (yyvsp[(1) - (2)]); append((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 106:
#line 682 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 107:
#line 684 "parser.bison"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 108:
#line 689 "parser.bison"
    {
         (yyval) = node(switch_element);
         field((yyval), label_list, (yyvsp[(1) - (2)]));
         field((yyval), statement_list, (yyvsp[(2) - (2)])); ;}
    break;

  case 109:
#line 697 "parser.bison"
    { (yyval) = list((yyvsp[(1) - (1)])); ;}
    break;

  case 110:
#line 699 "parser.bison"
    { (yyval) = (yyvsp[(1) - (2)]); append((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); ;}
    break;

  case 111:
#line 704 "parser.bison"
    {
			(yyval) = node(int_constant_expr);
			field((yyval), value, (yyvsp[(1) - (1)]));
		;}
    break;

  case 112:
#line 709 "parser.bison"
    {
			(yyval) = node(float_constant_expr);
			field((yyval), value, (yyvsp[(1) - (1)]));
		;}
    break;

  case 113:
#line 714 "parser.bison"
    {
			(yyval) = (yyvsp[(1) - (1)]);
		;}
    break;

  case 114:
#line 721 "parser.bison"
    {
			(yyval) = node(switch_label);
			field((yyval), test_constant, (yyvsp[(2) - (3)]));
		;}
    break;

  case 115:
#line 729 "parser.bison"
    {
			(yyval) = node(if_stmt);
			field((yyval), test_expression, (yyvsp[(2) - (4)]));
			field((yyval), if_block, (yyvsp[(4) - (4)]));
		;}
    break;

  case 116:
#line 735 "parser.bison"
    {
			(yyval) = node(if_stmt);
			field((yyval), test_expression, (yyvsp[(2) - (7)]));
			field((yyval), if_block, (yyvsp[(4) - (7)]));
			field((yyval), else_block, (yyvsp[(7) - (7)]));
		;}
    break;

  case 117:
#line 745 "parser.bison"
    {
			(yyval) = node(while_stmt);
			field((yyval), test_expression, (yyvsp[(2) - (4)]));
			field((yyval), statement_list, (yyvsp[(4) - (4)]));
		;}
    break;

  case 118:
#line 754 "parser.bison"
    {
			(yyval) = node(do_while_stmt);
			field((yyval), statement_list, (yyvsp[(3) - (6)]));
			field((yyval), test_expression, (yyvsp[(5) - (6)]));
		;}
    break;

  case 119:
#line 762 "parser.bison"
    {
			(yyval) = node(for_stmt);
			field((yyval), test_expression, (yyvsp[(4) - (8)]));
			field((yyval), init_expression, (yyvsp[(2) - (8)]));
			field((yyval), end_loop_expression, (yyvsp[(6) - (8)]));
			field((yyval), statement_list, (yyvsp[(8) - (8)]));
		;}
    break;

  case 120:
#line 770 "parser.bison"
    {
			(yyval) = node(for_stmt);
			field((yyval), variable_initializer, (yyvsp[(3) - (12)]));
			field((yyval), variable_type_spec, (yyvsp[(4) - (12)]));
			parse_node_ptr var_node = node(locator_expr);
			field(var_node, string, (yyvsp[(3) - (12)]));
			parse_node_ptr assignment_expr = node(assign_expr);
			field(assignment_expr, left, var_node);
			field(assignment_expr, right, (yyvsp[(6) - (12)]));
			field((yyval), init_expression, assignment_expr);

			field((yyval), test_expression, (yyvsp[(8) - (12)]));
			field((yyval), end_loop_expression, (yyvsp[(10) - (12)]));
			field((yyval), statement_list, (yyvsp[(12) - (12)]));
		;}
    break;

  case 121:
#line 793 "parser.bison"
    {
         (yyval) = node(locator_expr);
         field((yyval), string, (yyvsp[(1) - (1)]));
      ;}
    break;

  case 122:
#line 798 "parser.bison"
    {
         (yyval) = node(locator_expr);
         field((yyval), string, (yyvsp[(1) - (1)]));
      ;}
    break;

  case 123:
#line 803 "parser.bison"
    {
         (yyval) = node(int_constant_expr);
         field((yyval), value, (yyvsp[(1) - (1)]));
      ;}
    break;

  case 125:
#line 809 "parser.bison"
    {
         (yyval) = node(float_constant_expr);
         field((yyval), value, (yyvsp[(1) - (1)]));
      ;}
    break;

  case 126:
#line 814 "parser.bison"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 127:
#line 816 "parser.bison"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 130:
#line 823 "parser.bison"
    {
			(yyval) = node(method_call);
			field((yyval), object_expr, (yyvsp[(2) - (5)]));
			field((yyval), primary_name, (yyvsp[(3) - (5)]));
			field((yyval), selector_list, (yyvsp[(4) - (5)]));
		;}
    break;

  case 131:
#line 830 "parser.bison"
    {
			(yyval) = node(method_call);
			field((yyval), object_expr, (yyvsp[(2) - (4)]));
			field((yyval), primary_name, (yyvsp[(3) - (4)]));
			field((yyval), selector_list, empty_list());
		;}
    break;

  case 132:
#line 840 "parser.bison"
    {
			YYSTYPE pair;
			pair = node(selector_pair);
			field(pair, string, nil);
			field(pair, expr, (yyvsp[(2) - (2)]));
			(yyval) = list(pair);
		;}
    break;

  case 133:
#line 848 "parser.bison"
    {
			YYSTYPE pair;
			pair = node(selector_pair);
			field(pair, string, nil);
			field(pair, expr, (yyvsp[(3) - (3)]));
			append((yyvsp[(1) - (3)]), pair);
			(yyval) = (yyvsp[(1) - (3)]);
		;}
    break;

  case 134:
#line 857 "parser.bison"
    {
			YYSTYPE pair;
			pair = node(selector_pair);
			field(pair, string, (yyvsp[(2) - (4)]));
			field(pair, expr, (yyvsp[(4) - (4)]));
			append((yyvsp[(1) - (4)]), pair);
			(yyval) = (yyvsp[(1) - (4)]);
		;}
    break;

  case 135:
#line 869 "parser.bison"
    {
		  if((yyvsp[(2) - (3)]))
		  {
		     (yyval) = node(strcat_expr);
           YYSTYPE frag = node(string_constant);
           field(frag, value, (yyvsp[(1) - (3)]));
           field((yyval), left, frag);
           field((yyval), right, (yyvsp[(2) - (3)]));
		   field((yyval), op, string("cat_none"));
		  }
		  else
		  {
		     (yyval) = node(string_constant);
           field((yyval), value, (yyvsp[(1) - (3)]));
		  }
	   ;}
    break;

  case 136:
#line 889 "parser.bison"
    { (yyval) = nil; ;}
    break;

  case 137:
#line 891 "parser.bison"
    { (yyval) = (yyvsp[(1) - (1)]); ;}
    break;

  case 138:
#line 896 "parser.bison"
    {
         (yyval) = node(string_constant);
         field((yyval), value, (yyvsp[(1) - (1)])); ;}
    break;

  case 139:
#line 900 "parser.bison"
    {
         (yyval) = node(locator_expr);
         field((yyval), string, (yyvsp[(1) - (1)]));
      ;}
    break;

  case 140:
#line 905 "parser.bison"
    {
         (yyval) = node(strcat_expr);
         YYSTYPE frag = node(string_constant);
         field(frag, value, (yyvsp[(2) - (2)]));
         field((yyval), left, (yyvsp[(1) - (2)]));
         field((yyval), right, frag);
		 field((yyval), op, string("cat_none"));
      ;}
    break;

  case 141:
#line 914 "parser.bison"
    {
         (yyval) = node(strcat_expr);
         field((yyval), left, (yyvsp[(1) - (2)]));
         YYSTYPE ident = node(locator_expr);
         field(ident, string, (yyvsp[(2) - (2)]));
         field((yyval), right, ident);
		 field((yyval), op, string("cat_none"));
      ;}
    break;

  case 143:
#line 927 "parser.bison"
    {
         (yyval) = node(array_index_expr);
         field((yyval), array_expr, (yyvsp[(1) - (4)]));
         field((yyval), index_expr, (yyvsp[(3) - (4)]));
      ;}
    break;

  case 144:
#line 933 "parser.bison"
    {
         (yyval) = node(func_call_expr);
         field((yyval), func_expr, (yyvsp[(1) - (3)]));
         field((yyval), args, empty_list());
      ;}
    break;

  case 145:
#line 939 "parser.bison"
    {
         (yyval) = node(func_call_expr);
         field((yyval), func_expr, (yyvsp[(1) - (4)]));
         field((yyval), args, (yyvsp[(3) - (4)]));
      ;}
    break;

  case 146:
#line 945 "parser.bison"
    {
         (yyval) = node(slot_expr);
         field((yyval), object_expr, (yyvsp[(1) - (3)]));
         field((yyval), slot_name, (yyvsp[(3) - (3)]));
      ;}
    break;

  case 147:
#line 951 "parser.bison"
    {
         (yyval) = node(unary_lvalue_op_expr);
         field((yyval), expression, (yyvsp[(1) - (2)]));
         field((yyval), op, string("post_increment"));
      ;}
    break;

  case 148:
#line 957 "parser.bison"
    {
         (yyval) = node(unary_lvalue_op_expr);
         field((yyval), expression, (yyvsp[(1) - (2)]));
         field((yyval), op, string("post_decrement"));
      ;}
    break;

  case 149:
#line 966 "parser.bison"
    { (yyval) = list((yyvsp[(1) - (1)])); ;}
    break;

  case 150:
#line 968 "parser.bison"
    { (yyval) = (yyvsp[(1) - (3)]); append((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 152:
#line 974 "parser.bison"
    {
         (yyval) = node(unary_lvalue_op_expr);
         field((yyval), expression, (yyvsp[(2) - (2)]));
         field((yyval), op, string("pre_increment"));
      ;}
    break;

  case 153:
#line 980 "parser.bison"
    {
         (yyval) = node(unary_lvalue_op_expr);
         field((yyval), expression, (yyvsp[(2) - (2)]));
         field((yyval), op, string("pre_decrement"));
      ;}
    break;

  case 154:
#line 986 "parser.bison"
    {
         (yyval) = node(unary_minus_expr);
         field((yyval), expression, (yyvsp[(2) - (2)]));
      ;}
    break;

  case 155:
#line 991 "parser.bison"
    {
         (yyval) = node(logical_not_expr);
         field((yyval), expression, (yyvsp[(2) - (2)]));
      ;}
    break;

  case 156:
#line 996 "parser.bison"
    {
         (yyval) = node(bitwise_not_expr);
         field((yyval), expression, (yyvsp[(2) - (2)]));
      ;}
    break;

  case 158:
#line 1005 "parser.bison"
    {
         (yyval) = node(float_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("multiply"));
      ;}
    break;

  case 159:
#line 1012 "parser.bison"
    {
         (yyval) = node(float_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("divide"));
      ;}
    break;

  case 160:
#line 1019 "parser.bison"
    {
         (yyval) = node(int_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("modulus"));
      ;}
    break;

  case 162:
#line 1030 "parser.bison"
    {
         (yyval) = node(float_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("add"));
      ;}
    break;

  case 163:
#line 1037 "parser.bison"
    {
         (yyval) = node(float_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("subtract")); ;}
    break;

  case 165:
#line 1047 "parser.bison"
    {
         (yyval) = node(int_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("shift_left"));
      ;}
    break;

  case 166:
#line 1054 "parser.bison"
    {
         (yyval) = node(int_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("shift_right"));
      ;}
    break;

  case 168:
#line 1065 "parser.bison"
    {
         (yyval) = node(bool_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("compare_less"));
      ;}
    break;

  case 169:
#line 1072 "parser.bison"
    {
         (yyval) = node(bool_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("compare_greater"));
      ;}
    break;

  case 170:
#line 1079 "parser.bison"
    {
         (yyval) = node(bool_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("compare_less_or_equal"));
      ;}
    break;

  case 171:
#line 1086 "parser.bison"
    {
         (yyval) = node(bool_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("compare_greater_or_equal"));
      ;}
    break;

  case 173:
#line 1097 "parser.bison"
    {
         (yyval) = node(bool_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("compare_equal"));
      ;}
    break;

  case 174:
#line 1104 "parser.bison"
    {
         (yyval) = node(bool_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("compare_not_equal"));
      ;}
    break;

  case 176:
#line 1115 "parser.bison"
    {
         (yyval) = node(int_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("bitwise_and"));
      ;}
    break;

  case 178:
#line 1126 "parser.bison"
    {
         (yyval) = node(int_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("bitwise_xor"));
      ;}
    break;

  case 180:
#line 1137 "parser.bison"
    {
         (yyval) = node(int_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("bitwise_or"));
      ;}
    break;

  case 182:
#line 1148 "parser.bison"
    {
         (yyval) = node(bool_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("logical_and"));
      ;}
    break;

  case 184:
#line 1159 "parser.bison"
    {
         (yyval) = node(bool_binary_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("logical_or"));
      ;}
    break;

  case 186:
#line 1170 "parser.bison"
    {
         (yyval) = node(strcat_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("cat_none"));
      ;}
    break;

  case 187:
#line 1177 "parser.bison"
    {
         (yyval) = node(strcat_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("cat_newline"));
      ;}
    break;

  case 188:
#line 1184 "parser.bison"
    {
         (yyval) = node(strcat_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("cat_space"));
      ;}
    break;

  case 189:
#line 1191 "parser.bison"
    {
         (yyval) = node(strcat_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, string("cat_tab"));
      ;}
    break;

  case 193:
#line 1207 "parser.bison"
    {
         (yyval) = node(conditional_expr);
         field((yyval), test_expression, (yyvsp[(1) - (5)]));
         field((yyval), true_expression, (yyvsp[(3) - (5)]));
         field((yyval), false_expression, (yyvsp[(5) - (5)]));
      ;}
    break;

  case 195:
#line 1218 "parser.bison"
    {
         (yyval) = node(assign_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
      ;}
    break;

  case 196:
#line 1224 "parser.bison"
    {
         (yyval) = node(float_assign_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, (yyvsp[(2) - (3)]));
      ;}
    break;

  case 197:
#line 1231 "parser.bison"
    {
         (yyval) = node(int_assign_expr);
         field((yyval), left, (yyvsp[(1) - (3)]));
         field((yyval), right, (yyvsp[(3) - (3)]));
         field((yyval), op, (yyvsp[(2) - (3)]));
      ;}
    break;

  case 198:
#line 1241 "parser.bison"
    { (yyval) = string("multiply"); ;}
    break;

  case 199:
#line 1243 "parser.bison"
    { (yyval) = string("divide"); ;}
    break;

  case 200:
#line 1245 "parser.bison"
    { (yyval) = string("add"); ;}
    break;

  case 201:
#line 1247 "parser.bison"
    { (yyval) = string("subtract"); ;}
    break;

  case 202:
#line 1252 "parser.bison"
    { (yyval) = string("modulus"); ;}
    break;

  case 203:
#line 1254 "parser.bison"
    { (yyval) = string("shift_left"); ;}
    break;

  case 204:
#line 1256 "parser.bison"
    { (yyval) = string("shift_right"); ;}
    break;

  case 205:
#line 1258 "parser.bison"
    { (yyval) = string("bitwise_and"); ;}
    break;

  case 206:
#line 1260 "parser.bison"
    { (yyval) = string("bitwise_xor"); ;}
    break;

  case 207:
#line 1262 "parser.bison"
    { (yyval) = string("bitwise_or"); ;}
    break;

  case 209:
#line 1271 "parser.bison"
    {
			(yyval) = node(expression_stmt);
			field((yyval), expr, (yyvsp[(1) - (1)]));
		;}
    break;

  case 210:
#line 1279 "parser.bison"
    {
		 (yyval) = node(array_expr);
		 field((yyval), array_values, empty_list());
		;}
    break;

  case 211:
#line 1284 "parser.bison"
    {
         (yyval) = node(array_expr);
         field((yyval), array_values, (yyvsp[(2) - (3)]));
      ;}
    break;

  case 212:
#line 1289 "parser.bison"
    {
         (yyval) = node(array_expr);
         field((yyval), array_values, (yyvsp[(2) - (4)]));
      ;}
    break;

  case 213:
#line 1297 "parser.bison"
    { (yyval) = node(map_expr); ;}
    break;

  case 214:
#line 1299 "parser.bison"
    {
         (yyval) = node(map_expr);
         field((yyval), map_pairs, (yyvsp[(2) - (3)]));
      ;}
    break;

  case 215:
#line 1304 "parser.bison"
    {
         (yyval) = node(map_expr);
         field((yyval), map_pairs, (yyvsp[(2) - (4)]));
      ;}
    break;

  case 216:
#line 1312 "parser.bison"
    { (yyval) = list((yyvsp[(1) - (1)])); ;}
    break;

  case 217:
#line 1314 "parser.bison"
    { (yyval) = (yyvsp[(1) - (3)]); append((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 218:
#line 1319 "parser.bison"
    { (yyval) = list((yyvsp[(1) - (1)])); ;}
    break;

  case 219:
#line 1321 "parser.bison"
    { (yyval) = (yyvsp[(1) - (3)]); append((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); ;}
    break;

  case 220:
#line 1326 "parser.bison"
    {
         (yyval) = node(map_pair);
         field((yyval), key, (yyvsp[(1) - (3)]));
         field((yyval), value, (yyvsp[(3) - (3)]));
      ;}
    break;

  case 221:
#line 1335 "parser.bison"
    {
         (yyval) = node(function_expr);
         field((yyval), parameter_list, (yyvsp[(3) - (8)]));
         field((yyval), return_type, (yyvsp[(5) - (8)]));
         field((yyval), expr, (yyvsp[(7) - (8)]));
      ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3434 "parser.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, lexer, result, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (&yylloc, lexer, result, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, lexer, result, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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
		      yytoken, &yylval, &yylloc, lexer, result);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, lexer, result);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, lexer, result, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, lexer, result);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, lexer, result);
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1343 "parser.bison"


void kt_error (struct YYLTYPE *loc, kt_lexer *lexer, parse_result *result, const char *format, ...)
{
	result->error = true;
	va_list args;
	va_start(args, format);
	result->error_string.format(format, args);
	va_end(args);
	result->error_line_number = loc->first_line;
	result->error_column_number = loc->first_column;
}

void parser_init()
{
    //yydebug = 0;
    
}


