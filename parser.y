%{
#include <stdio.h>
#include <stdint.h>
#include "scanner.h"
#include "memory.h"

/*
 * Global root node.
 */

//#define USE_PARSE_TRACE
#ifdef USE_PARSE_TRACE
#   define TRACE(...) do { \
            printf(">>> %d: ", yylineno); \
            printf(__VA_ARGS__); \
            fputc('\n', stdout); \
        } while(0)
#else
#   define TRACE(...)
#endif

void yyerror(const char*);

%}

%union {
    const char* text;
    int64_t integer_lit;
    double float_lit;
};

%token <text> IDENTIFIER
%token <text> STRING_LIT
%token <integer_lit> INTEGER_LIT
%token <float_lit> FLOAT_LIT

%token INTEGER FLOAT STRING LIST HASH FILER FILEW FILERW NOTHING
%token IF ELSE WHILE DO FOR TRY EXCEPT FINAL SWITCH CASE DEFAULT
%token EQU_OPER NEQ_OPER LTE_OPER GTE_OPER LT_OPER GT_OPER OR_OPER AND_OPER
%token ASSIGN_OPER ADD_OPER SUB_OPER MUL_OPER DIV_OPER MOD_OPER POW_OPER NEG_OPER
%token OPAREN CPAREN OCBRACE CCBRACE OSBRACE CSBRACE COMMA DOT
%token BREAK CONTINUE RETURN PRINT TRACE EXIT

%define lr.type canonical-lr
%define parse.lac full
%define parse.error detailed
%locations
%verbose
%header "parser.h"
%output "parser.c"

%%

program
    : program_item_list {}
    ;

program_item_list
    : program_item {}
    | program_item_list program_item {}
    ;

program_item
    : data_definition {}
    | func_definition {}
    ;

type_name
    : INTEGER {}
    | FLOAT {}
    | STRING {}
    | LIST {}
    | HASH {}
    | FILER {}
    | FILEW {}
    | FILERW {}
    | NOTHING {}
    ;

data_declaration
    : type_name IDENTIFIER
    ;

data_declaration_list
    : data_declaration {}
    | data_declaration_list COMMA data_declaration {}
    ;

data_definition
    : data_declaration {}
    | data_declaration ASSIGN_OPER expression {}
    ;

func_definition
    : data_declaration func_params func_body {}
    ;

func_params
    : OPAREN data_declaration_list CPAREN {}
    | OPAREN CPAREN {}
    | OPAREN NOTHING CPAREN {}
    ;

func_body
    : OCBRACE CCBRACE {}
    | OCBRACE func_body_list CCBRACE {}
    ;

func_body_list
    : func_body_elem {}
    | func_body_list func_body_elem {}
    ;

func_body_elem
    : %empty
    ;

expression
    : %empty
    ;

%%

void yyerror(const char* s) {

    fprintf(stderr, "error:%d %s\n", yylineno, s);
    //errors++;
}

const char* tokenToStr(int tok) {

    return yysymbol_name(YYTRANSLATE(tok));
}
