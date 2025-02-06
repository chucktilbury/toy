%{
#include <stdio.h>
#include <stdint.h>

#include "scanner.h"
#include "memory.h"
#include "fileio.h"
#include "errors.h"
//#include "ast.h"
//#include "symtab.h"

// #define USE_PARSE_TRACE

#ifdef USE_PARSE_TRACE
#define TRACE(...) do { \
        printf("TRACE:%s:%d:%d: ", \
                get_file_name(), get_line_no(), get_col_no()); \
        printf(__VA_ARGS__); \
        fputc('\n', stdout); \
    } while(0)
#else
#define TRACE(...)
#endif

void yyerror(const char*);

/*
 * Global root node is the return value of the parser.
 * Defined in ast.c
 */
//extern ast_node_t* root_node;

%}

// support the typedefs in the %union.
%code requires {
#include <stdint.h>
#include <stdbool.h>
//#include "ast.h"
#include "tokens.h"
}

// this goes at the bottom of the generated header file.
%code provides {
const char* token_to_str(int);
}

%union {
    token_t* token;
    //ast_node_t* node;
};

%token <token> IDENTIFIER STRING_LIT INTEGER_LIT FLOAT_LIT
%token <token> INTEGER FLOAT STRING LIST HASH NOTHING
%token <token> EQU_OPER NEQ_OPER LTE_OPER GTE_OPER LT_OPER
%token <token> NOT_OPER OR_OPER AND_OPER GT_OPER UNARY_MINUS_OPER
%token <token> ADD_OPER SUB_OPER MUL_OPER DIV_OPER MOD_OPER POW_OPER DOT_OPER

%token IF ELSE WHILE DO TRY EXCEPT RAISE IMPORT YIELD FOR IN
%token BREAK CONTINUE RETURN PRINT TRACE EXIT CONST ITERATOR

    /*
    %nterm <node> program program_item_list program_item type_name formatted_string
    %nterm <node> import_statement  func_name assignment_left
    %nterm <node> data_declaration_list data_definition func_definition func_params
    %nterm <node> func_body func_body_list loop_body loop_body_list loop_body_diffs
    %nterm <node> loop_body_elem func_body_elem trace_statement print_statement
    %nterm <node> exit_statement return_statement tryexcept_statement try_clause
    %nterm <node> except_segment except_clause_list final_except_clause except_clause
    %nterm <node> raise_statement if_clause ifelse_statement else_segment
    %nterm <node> final_else_segment else_clause_list else_clause while_clause
    %nterm <node> while_statement do_statement assignment_right assignment raw_value
    %nterm <node> expr_primary list_reference list_ref_value list_ref_param
    %nterm <node> list_ref_param_list func_reference expression expression_list
    %nterm <node> expression_list_param expression_param list_init dict_init
    %nterm <node> dict_init_item dict_init_item_list  for_statement
    %nterm <node> data_declaration exception_identifier
    */

    //%define lr.type canonical-lr
    // %define lr.type ielr
%define parse.lac full
%define parse.error detailed
%locations
%verbose
%header "parser.h"
%output "parser.c"

%left OR_OPER
%left AND_OPER
%left EQU_OPER NEQ_OPER
%left LTE_OPER GTE_OPER LT_OPER GT_OPER
%left ADD_OPER SUB_OPER
%left MUL_OPER DIV_OPER MOD_OPER
%right NOT_OPER POW_OPER
%right UNARY
%left DOT_OPER

%%

program
    : program_item_list
    ;

program_item_list
    : program_item
    | program_item_list program_item
    ;

import_statement
    : IMPORT STRING_LIT
    ;

program_item
    : data_definition
    | func_definition
    | func_body
    | import_statement
    | exception_identifier
    ;

    /*
     * Create a exception handler ID symbol.
     */
exception_identifier
    : EXCEPT IDENTIFIER
    ;

type_name
    : INTEGER
    | FLOAT
    | STRING
    | LIST
    | HASH
    ;

formatted_string
    : STRING_LIT expression_list_param
    | STRING_LIT
    ;

    /*
     * Generate a data definition symbol.
     */
data_declaration
    : type_name IDENTIFIER
    | CONST type_name IDENTIFIER
    ;

data_declaration_list
    : data_declaration
    | data_declaration_list ',' data_declaration
    ;

data_definition
    : data_declaration
    | data_declaration '=' expression
    | data_declaration '=' list_init
    | data_declaration '=' dict_init
    ;

    /*
     * Generate a function name symbol, but the decorated symbol name
     * cannot be generated until the parameters have been parsed.
     */
func_name
    : type_name IDENTIFIER
    | ITERATOR type_name IDENTIFIER
    | NOTHING IDENTIFIER
    ;

    /*
     * Add the parameters in their own symbol context and finish generating
     * the decorated function name.
     */
func_definition
    : func_name func_params func_body
    ;

func_params
    : '(' data_declaration_list ')'
    | '(' ')'
    ;

func_body
    : '{' func_body_list '}'
    ;

func_body_list
    : func_body_elem
    | func_body_list func_body_elem
    ;

loop_body
    : '{' '}'
    | '{' loop_body_list '}'
    ;

loop_body_list
    : loop_body_elem
    | loop_body_list loop_body_elem
    ;

loop_body_diffs
    : BREAK
    | CONTINUE
    | YIELD '(' expression ')'
    ;

loop_body_elem
    : func_body_elem
    | loop_body_diffs
    ;

func_body_elem
    : data_definition
    | func_reference
    | assignment
    | while_statement
    | do_statement
    | for_statement
    | ifelse_statement
    | tryexcept_statement
    | raise_statement
    | return_statement
    | exit_statement
    | print_statement
    | trace_statement
    | func_body
    ;

trace_statement
    : TRACE expression_list_param
    | TRACE
    ;

print_statement
    : PRINT expression_list_param
    | PRINT
    ;

exit_statement
    : EXIT expression_param
    ;

return_statement
    : RETURN expression_param
    | RETURN
    ;

    /*
     * If the type name is specified then this is a symbol definition, but
     * if not, then it is a symbol reference. Either way it gets it's own
     * symbol context similar to a function definition.
     */
for_statement
    : FOR '(' IDENTIFIER IN expression ')' func_body
    | FOR '(' type_name IDENTIFIER IN expression ')' func_body
    ;

tryexcept_statement
    : try_clause except_clause
    ;

try_clause
    : TRY func_body
    ;

    /*
     * The first identifier is a reference to an expression handler ID and the
     * second identifier is a symbol definition of a string that requires its
     * own symbol context, similar to a function definition.
     */
except_segment
    : EXCEPT '(' IDENTIFIER ',' IDENTIFIER ')' func_body
    ;

except_clause_list
    : except_segment
    | except_clause_list except_segment
    ;

    /*
     * The identifier is a definition of a string symbol, same as the second
     * identifier in a except_segment.
     */
final_except_clause
    : EXCEPT '(' IDENTIFIER ')' func_body
    ;

except_clause
    : except_clause_list
    | except_clause_list final_except_clause
    | final_except_clause
    ;

    /*
     * The identifier is a reference to an exception ID and the string is
     * assigned to the second identifier in an except clause.
     */
raise_statement
    : RAISE '(' IDENTIFIER ',' formatted_string ')'
    ;

if_clause
    : IF '(' expression ')' func_body
    ;

ifelse_statement
    : if_clause
    | if_clause else_clause
    ;

else_segment
    : ELSE '(' expression ')' func_body
    ;

final_else_segment
    : ELSE '(' ')' func_body
    | ELSE func_body
    ;

else_clause_list
    : else_segment
    | else_clause_list else_segment
    ;

else_clause
    : else_clause_list
    | else_clause_list final_else_segment
    | final_else_segment
    ;

while_clause
    : WHILE expression_param
    | WHILE
    ;

while_statement
    : while_clause loop_body
    ;

do_statement
    : DO loop_body while_clause
    ;

assignment_right
    : expression
    | '(' type_name ')' expression
    | list_init
    | dict_init
    ;

list_init
    : '[' expression_list ']'
    ;

dict_init_item
    : STRING_LIT ':' expression
    ;

dict_init_item_list
    : dict_init_item
    | dict_init_item_list ',' dict_init_item
    ;

dict_init
    : '[' dict_init_item_list ']'
    ;

    /*
     * Identifier is a reference that has to be examined when the variable
     * references are verified.
     */
assignment_left
    : IDENTIFIER
    | list_reference
    ;

assignment
    : assignment_left '=' assignment_right
    ;

    /*
     * references only.
     *
    raw_value
    : IDENTIFIER
    | INTEGER_LIT
    | FLOAT_LIT
    ;

    expr_primary
    : raw_value
    | formatted_string
    | list_reference
    | func_reference
    | '(' expression ')'
    ;
    *.

    /*
     * This includes hashes as well as lists, since they are the same syntax.
     */
list_reference
    : IDENTIFIER list_ref_param_list
    ;

    /*
     * These have to be throurally check for symantic correctness.
     */
list_ref_value
    : IDENTIFIER
    | INTEGER_LIT
    | STRING_LIT
    | list_reference
    | func_reference
    | list_ref_param
    ;

list_ref_param
    : '[' list_ref_value ']'
    ;

list_ref_param_list
    : list_ref_param
    | list_ref_param_list list_ref_param
    ;

func_reference
    : DOT_OPER IDENTIFIER expression_list_param
    ;

compound_reference_element
    : IDENTIFIER
    | list_reference
    | func_reference
    ;

compound_reference
    : compound_reference_element
    | compound_reference DOT_OPER compound_reference_element
    ;

expression
    : compound_reference
    | INTEGER_LIT
    | FLOAT_LIT
    | formatted_string
    | expression ADD_OPER expression
    | expression SUB_OPER expression
    | expression MUL_OPER expression
    | expression DIV_OPER expression
    | expression MOD_OPER expression
    | expression POW_OPER expression
    | expression GT_OPER expression
    | expression LT_OPER expression
    | expression EQU_OPER expression
    | expression NEQ_OPER expression
    | expression LTE_OPER expression
    | expression GTE_OPER expression
    | expression OR_OPER expression
    | expression AND_OPER expression
    | NOT_OPER expression %prec UNARY
    | SUB_OPER expression %prec UNARY
    | '(' expression ')'
    ;

expression_list
    : expression
    | expression_list ',' expression
    ;

expression_list_param
    : '(' expression_list ')'
    | '(' ')'
    ;

expression_param
    : '(' expression ')'
    | '(' ')'
    ;

%%

void yyerror(const char* s) {

    parser_syntax_error("%s", s);
}

const char* token_to_str(int tok) {

    return yysymbol_name(YYTRANSLATE(tok));
}
