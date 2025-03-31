/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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


