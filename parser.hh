/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     FUNCTION = 258,
     ID = 259,
     DECLARE = 260,
     ARRAY = 261,
     INTEGER = 262,
     OF = 263,
     REAL = 264,
     XBEGIN = 265,
     XEND = 266,
     IF = 267,
     THEN = 268,
     ELSE = 269,
     WHILE = 270,
     DO = 271,
     ASSIGN = 272,
     RETURN = 273,
     GE = 274,
     LE = 275,
     EQ = 276,
     NE = 277,
     TRUE = 278,
     FALSE = 279,
     PROGRAM = 280,
     ELSEIF = 281,
     AND = 282,
     OR = 283,
     NOT = 284
   };
#endif
#define FUNCTION 258
#define ID 259
#define DECLARE 260
#define ARRAY 261
#define INTEGER 262
#define OF 263
#define REAL 264
#define XBEGIN 265
#define XEND 266
#define IF 267
#define THEN 268
#define ELSE 269
#define WHILE 270
#define DO 271
#define ASSIGN 272
#define RETURN 273
#define GE 274
#define LE 275
#define EQ 276
#define NE 277
#define TRUE 278
#define FALSE 279
#define PROGRAM 280
#define ELSEIF 281
#define AND 282
#define OR 283
#define NOT 284




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 37 "parser.y"
typedef union YYSTYPE {
    ASTNode                 *ast;
    Expression              *expression;
    ExpressionList          *expressionList;
    Statement               *statement;
    StatementList           *statementList;
    Condition               *condition;
    ArrayReference          *aref;
    FunctionCall            *call;
    LeftValue               *lvalue;
    ElseIfList              *elseIfList;
    
    VariableInformation     *variable;
    TypeInformation         *type;
    FunctionInformation     *function;

    string                  *id;
    int                      integer;
    double                   real;
    void                    *null;
} YYSTYPE;
/* Line 1318 of yacc.c.  */
#line 117 "parser.hh"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



