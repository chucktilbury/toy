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

#ifndef YY_YY_GRAMMAR_H_INCLUDED
# define YY_YY_GRAMMAR_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    CONST = 258,                   /* CONST  */
    NOTHING = 259,                 /* NOTHING  */
    IMPORT = 260,                  /* IMPORT  */
    INT = 261,                     /* INT  */
    FLOAT = 262,                   /* FLOAT  */
    STRING = 263,                  /* STRING  */
    BOOL = 264,                    /* BOOL  */
    LIST = 265,                    /* LIST  */
    DICT = 266,                    /* DICT  */
    STRUCT = 267,                  /* STRUCT  */
    DO = 268,                      /* DO  */
    WHILE = 269,                   /* WHILE  */
    FOR = 270,                     /* FOR  */
    IN = 271,                      /* IN  */
    IF = 272,                      /* IF  */
    ELSE = 273,                    /* ELSE  */
    RETURN = 274,                  /* RETURN  */
    EXIT = 275,                    /* EXIT  */
    IDENTIFIER = 276,              /* IDENTIFIER  */
    INT_LITERAL = 277,             /* INT_LITERAL  */
    FLOAT_LITERAL = 278,           /* FLOAT_LITERAL  */
    STRING_LITERAL = 279,          /* STRING_LITERAL  */
    INLINE = 280,                  /* INLINE  */
    COLON = 281,                   /* COLON  */
    OR = 282,                      /* OR  */
    AND = 283,                     /* AND  */
    EQU = 284,                     /* EQU  */
    NEQU = 285,                    /* NEQU  */
    GT = 286,                      /* GT  */
    LT = 287,                      /* LT  */
    GTE = 288,                     /* GTE  */
    LTE = 289,                     /* LTE  */
    PLUS = 290,                    /* PLUS  */
    MINUS = 291,                   /* MINUS  */
    STAR = 292,                    /* STAR  */
    SLASH = 293,                   /* SLASH  */
    PERCENT = 294,                 /* PERCENT  */
    CARAT = 295,                   /* CARAT  */
    NOT = 296,                     /* NOT  */
    DOT = 297,                     /* DOT  */
    OSBRACE = 298,                 /* OSBRACE  */
    CSBRACE = 299,                 /* CSBRACE  */
    COMMA = 300,                   /* COMMA  */
    OCBRACE = 301,                 /* OCBRACE  */
    CCBRACE = 302,                 /* CCBRACE  */
    EQUAL = 303,                   /* EQUAL  */
    OPAREN = 304,                  /* OPAREN  */
    CPAREN = 305,                  /* CPAREN  */
    CONTINUE = 306,                /* CONTINUE  */
    BREAK = 307,                   /* BREAK  */
    UNARY = 308                    /* UNARY  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_GRAMMAR_H_INCLUDED  */
