/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "jack.y"

    /* defintions */
    #include <cstddef>
    #include <cstdint>
    #include <string>
    #include <iostream>

#line 57 "parser.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOK_INTEGER = 258,             /* TOK_INTEGER  */
    TOK_STRING = 259,              /* TOK_STRING  */
    TOK_IDENT = 260,               /* TOK_IDENT  */
    TOK_CLASS = 261,               /* TOK_CLASS  */
    TOK_CONSTRUCTOR = 262,         /* TOK_CONSTRUCTOR  */
    TOK_FUNCTION = 263,            /* TOK_FUNCTION  */
    TOK_METHOD = 264,              /* TOK_METHOD  */
    TOK_FIELD = 265,               /* TOK_FIELD  */
    TOK_STATIC = 266,              /* TOK_STATIC  */
    TOK_VAR = 267,                 /* TOK_VAR  */
    TOK_INT = 268,                 /* TOK_INT  */
    TOK_CHAR = 269,                /* TOK_CHAR  */
    TOK_BOOLEAN = 270,             /* TOK_BOOLEAN  */
    TOK_VOID = 271,                /* TOK_VOID  */
    TOK_TRUE = 272,                /* TOK_TRUE  */
    TOK_FALSE = 273,               /* TOK_FALSE  */
    TOK_NULL = 274,                /* TOK_NULL  */
    TOK_THIS = 275,                /* TOK_THIS  */
    TOK_LET = 276,                 /* TOK_LET  */
    TOK_DO = 277,                  /* TOK_DO  */
    TOK_IF = 278,                  /* TOK_IF  */
    TOK_ELSE = 279,                /* TOK_ELSE  */
    TOK_WHILE = 280,               /* TOK_WHILE  */
    TOK_RETURN = 281,              /* TOK_RETURN  */
    TOK_LBRACE = 282,              /* TOK_LBRACE  */
    TOK_RBRACE = 283,              /* TOK_RBRACE  */
    TOK_LPAREN = 284,              /* TOK_LPAREN  */
    TOK_RPAREN = 285,              /* TOK_RPAREN  */
    TOK_LBRACKET = 286,            /* TOK_LBRACKET  */
    TOK_RBRACKET = 287,            /* TOK_RBRACKET  */
    TOK_DOT = 288,                 /* TOK_DOT  */
    TOK_COMMA = 289,               /* TOK_COMMA  */
    TOK_SEMICOLON = 290,           /* TOK_SEMICOLON  */
    TOK_PLUS = 291,                /* TOK_PLUS  */
    TOK_MINUS = 292,               /* TOK_MINUS  */
    TOK_ASTERISK = 293,            /* TOK_ASTERISK  */
    TOK_FSLASH = 294,              /* TOK_FSLASH  */
    TOK_AMPERSAND = 295,           /* TOK_AMPERSAND  */
    TOK_PIPE = 296,                /* TOK_PIPE  */
    TOK_LANGLE = 297,              /* TOK_LANGLE  */
    TOK_RANGLE = 298,              /* TOK_RANGLE  */
    TOK_EQUALS = 299,              /* TOK_EQUALS  */
    TOK_TILDE = 300                /* TOK_TILDE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 9 "jack.y"

    int16_t num;
    std::string str;

#line 124 "parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_H_INCLUDED  */
