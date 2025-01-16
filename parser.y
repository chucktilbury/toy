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
%token IF ELSE WHILE DO TRY EXCEPT FINAL RAISE
%token EQU_OPER NEQ_OPER LTE_OPER GTE_OPER LT_OPER GT_OPER OR_OPER AND_OPER
%token ASSIGN_OPER ADD_OPER SUB_OPER MUL_OPER DIV_OPER MOD_OPER POW_OPER NOT_OPER
%token OPAREN CPAREN OCBRACE CCBRACE OSBRACE CSBRACE COMMA DOT
%token BREAK CONTINUE RETURN PRINT TRACE EXIT

%define lr.type canonical-lr
%define parse.lac full
%define parse.error detailed
%locations
%verbose
%header "parser.h"
%output "parser.c"

%left UNARY
%left OR_OPER
%left AND_OPER
%left EQU_OPER NEQ_OPER
%left LTE_OPER GTE_OPER LT_OPER GT_OPER
%left ADD_OPER SUB_OPER
%left MUL_OPER DIV_OPER MOD_OPER
%right POW_OPER

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
    : type_name IDENTIFIER {}
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

loop_body
    : OCBRACE CCBRACE {}
    | OCBRACE loop_body_list CCBRACE {}
    ;

loop_body_list
    : loop_body_elem {}
    | loop_body_list loop_body_elem {}
    ;

loop_body_elem
    : func_body_elem {}
    | BREAK {}
    | CONTINUE {}
    ;

func_body_elem
    : data_definition {}
    | func_reference {}
    | assignment {}
    | while_statement {}
    | do_statement {}
    | ifelse_statement {}
    | tryexcept_statement {}
    | raise_statement {}
    | PRINT OPAREN expression_list CPAREN {}
    | TRACE OPAREN expression_list CPAREN {}
    | RETURN OPAREN expression CPAREN {}
    | EXIT OPAREN expression CPAREN {}
    ;

tryexcept_statement
    : TRY func_body except_clause {}
    ;

except_segment
    : EXCEPT OPAREN IDENTIFIER COMMA IDENTIFIER CPAREN func_body {}
    ;

final_except_segment
    : FINAL func_body {}
    ;

except_segment_list
    : except_segment {}
    | except_segment_list except_segment {}
    ;

except_clause
    : except_segment_list {}
    | except_segment_list final_except_segment {}
    ;

raise_statement
    : RAISE OPAREN IDENTIFIER COMMA STRING_LIT CPAREN {}
    ;

ifelse_statement
    : IF OPAREN expression CPAREN func_body else_clause {}
    ;

else_segment
    : ELSE OPAREN expression CPAREN func_body {}
    ;

final_else_segment
    : ELSE OPAREN CPAREN func_body {}
    | ELSE func_body {}
    ;

else_clause_list
    : else_segment {}
    | else_clause_list else_segment {}
    ;

else_clause
    : else_clause_list {}
    | else_clause_list final_else_segment {}
    ;

while_clause
    : WHILE OPAREN expression CPAREN {}
    | WHILE OPAREN CPAREN {}
    | WHILE {}
    ;

while_statement
    : while_clause loop_body {}
    ;

do_statement
    : DO loop_body while_clause {}
    ;

assignment
    : IDENTIFIER ASSIGN_OPER expression {}
    | list_reference ASSIGN_OPER expression {}
    ;

literal_value
    : IDENTIFIER {}
    | INTEGER_LIT {}
    | STRING_LIT {}
    | list_reference {}
    | func_reference {}
    | FLOAT_LIT {}
    ;

list_reference
    : IDENTIFIER list_ref_param_list {}
    ;

list_ref_value
    : IDENTIFIER {}
    | INTEGER_LIT {}
    | STRING_LIT {}
    | list_reference {}
    | func_reference {}
    | list_ref_param {}
    ;

list_ref_param
    : OSBRACE list_ref_value CSBRACE {}
    ;

list_ref_param_list
    : list_ref_param {}
    | list_ref_param_list list_ref_param {}
    ;

func_reference
    : IDENTIFIER func_ref_params {}
    ;

func_ref_params
    : OPAREN CPAREN {}
    | OPAREN expression_list CPAREN {}
    ;

expression
    : literal_value {}
    | expression ADD_OPER expression {}
    | expression SUB_OPER expression {}
    | expression MUL_OPER expression {}
    | expression DIV_OPER expression {}
    | expression MOD_OPER expression {}
    | expression EQU_OPER expression {}
    | expression NEQ_OPER expression {}
    | expression LTE_OPER expression {}
    | expression GTE_OPER expression {}
    | expression OR_OPER expression {}
    | expression AND_OPER expression {}
    | expression LT_OPER expression {}
    | expression GT_OPER expression {}
    | expression POW_OPER expression {}
    | SUB_OPER expression %prec UNARY {}
    | NOT_OPER expression %prec UNARY {}
    | '(' expression ')' {}
    ;

expression_list
    : expression {}
    | expression_list COMMA expression {}
    ;
%%

void yyerror(const char* s) {

    fprintf(stderr, "error:%d %s\n", yylineno, s);
    //errors++;
}

const char* tokenToStr(int tok) {

    return yysymbol_name(YYTRANSLATE(tok));
}
