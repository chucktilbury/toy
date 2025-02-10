%{
#include <stdio.h>
#include <stdint.h>

#include "scanner.h"
#include "memory.h"
#include "fileio.h"
#include "errors.h"
#include "ast.h"
//#include "symtab.h"

//#define USE_TRACE
#include "trace.h"

// #define USE_PARSE_TRACE

void yyerror(const char*);

/*
 * Global root node is the return value of the parser.
 * Defined in ast.c
 */
extern ast_program_t* root_node;

%}

// support the typedefs in the %union.
%code requires {
#include <stdint.h>
#include <stdbool.h>
#include "ast.h"
#include "tokens.h"
}

// this goes at the bottom of the generated header file.
%code provides {
const char* token_to_str(int);
}

%union {
    token_t* token;
    ast_node_t* node;
    ast_program_t* program;
    ast_program_item_list_t* program_item_list;
    ast_import_statement_t* import_statement;
    ast_program_item_t* program_item;
    ast_exception_identifier_t* exception_identifier;
    ast_type_name_t* type_name;
    ast_formatted_string_t* formatted_string;
    ast_data_declaration_t* data_declaration;
    ast_data_declaration_list_t* data_declaration_list;
    ast_data_definition_t* data_definition;
    ast_func_name_t* func_name;
    ast_func_definition_t* func_definition;
    ast_func_params_t* func_params;
    ast_func_body_t* func_body;
    ast_func_body_list_t* func_body_list;
    ast_loop_body_t* loop_body;
    ast_loop_body_list_t* loop_body_list;
    ast_loop_body_diffs_t* loop_body_diffs;
    ast_loop_body_elem_t* loop_body_elem;
    ast_func_body_elem_t* func_body_elem;
    ast_trace_statement_t* trace_statement;
    ast_inline_statement_t* inline_statement;
    ast_print_statement_t* print_statement;
    ast_exit_statement_t* exit_statement;
    ast_return_statement_t* return_statement;
    ast_for_statement_t* for_statement;
    ast_tryexcept_statement_t* tryexcept_statement;
    ast_try_clause_t* try_clause;
    ast_except_segment_t* except_segment;
    ast_except_clause_list_t* except_clause_list;
    ast_final_except_clause_t* final_except_clause;
    ast_except_clause_t* except_clause;
    ast_raise_statement_t* raise_statement;
    ast_if_clause_t* if_clause;
    ast_ifelse_statement_t* ifelse_statement;
    ast_else_segment_t* else_segment;
    ast_final_else_segment_t* final_else_segment;
    ast_else_clause_list_t* else_clause_list;
    ast_else_clause_t* else_clause;
    ast_while_clause_t* while_clause;
    ast_while_statement_t* while_statement;
    ast_do_statement_t* do_statement;
    ast_assignment_right_t* assignment_right;
    ast_list_init_t* list_init;
    ast_dict_init_item_t* dict_init_item;
    ast_dict_init_item_list_t* dict_init_item_list;
    ast_dict_init_t* dict_init;
    ast_assignment_left_t* assignment_left;
    ast_assignment_t* assignment;
    ast_raw_value_t* raw_value;
    ast_expr_primary_t* expr_primary;
    ast_list_reference_t* list_reference;
    ast_list_ref_value_t* list_ref_value;
    ast_list_ref_param_t* list_ref_param;
    ast_list_ref_param_list_t* list_ref_param_list;
    ast_func_reference_t* func_reference;
    ast_expression_t* expression;
    ast_expression_list_t* expression_list;
    ast_expression_list_param_t* expression_list_param;
    ast_expression_param_t* expression_param;
};

%token <token> IDENTIFIER STRING_LIT INTEGER_LIT FLOAT_LIT
%token <token> INTEGER FLOAT STRING LIST HASH NOTHING INLINE
%token <token> EQU_OPER NEQ_OPER LTE_OPER GTE_OPER LT_OPER
%token <token> NOT_OPER OR_OPER AND_OPER GT_OPER UNARY_MINUS_OPER
%token <token> ADD_OPER SUB_OPER MUL_OPER DIV_OPER MOD_OPER POW_OPER

%token IF ELSE WHILE DO TRY EXCEPT RAISE IMPORT YIELD FOR IN TYPE
%token BREAK CONTINUE RETURN PRINT TRACE EXIT CONST ITERATOR

%nterm <program> program
%nterm <program_item_list> program_item_list
%nterm <import_statement> import_statement
%nterm <program_item> program_item
%nterm <exception_identifier> exception_identifier
%nterm <type_name> type_name
%nterm <formatted_string> formatted_string
%nterm <data_declaration> data_declaration
%nterm <data_declaration_list> data_declaration_list
%nterm <data_definition> data_definition
%nterm <func_name> func_name
%nterm <func_definition> func_definition
%nterm <func_params> func_params
%nterm <func_body> func_body
%nterm <func_body_list> func_body_list
%nterm <loop_body> loop_body
%nterm <loop_body_list> loop_body_list
%nterm <loop_body_diffs> loop_body_diffs
%nterm <loop_body_elem> loop_body_elem
%nterm <func_body_elem> func_body_elem
%nterm <trace_statement> trace_statement
%nterm <inline_statement> inline_statement
%nterm <print_statement> print_statement
%nterm <exit_statement> exit_statement
%nterm <return_statement> return_statement
%nterm <for_statement> for_statement
%nterm <tryexcept_statement> tryexcept_statement
%nterm <try_clause> try_clause
%nterm <except_segment> except_segment
%nterm <except_clause_list> except_clause_list
%nterm <final_except_clause> final_except_clause
%nterm <except_clause> except_clause
%nterm <raise_statement> raise_statement
%nterm <if_clause> if_clause
%nterm <ifelse_statement> ifelse_statement
%nterm <else_segment> else_segment
%nterm <final_else_segment> final_else_segment
%nterm <else_clause_list> else_clause_list
%nterm <else_clause> else_clause
%nterm <while_clause> while_clause
%nterm <while_statement> while_statement
%nterm <do_statement> do_statement
%nterm <assignment_right> assignment_right
%nterm <list_init> list_init
%nterm <dict_init_item> dict_init_item
%nterm <dict_init_item_list> dict_init_item_list
%nterm <dict_init> dict_init
%nterm <assignment_left> assignment_left
%nterm <assignment> assignment
%nterm <raw_value> raw_value
%nterm <expr_primary> expr_primary
%nterm <list_reference> list_reference
%nterm <list_ref_value> list_ref_value
%nterm <list_ref_param> list_ref_param
%nterm <list_ref_param_list> list_ref_param_list
%nterm <func_reference> func_reference
%nterm <expression> expression
%nterm <expression_list> expression_list
%nterm <expression_list_param> expression_list_param
%nterm <expression_param> expression_param

    // %define lr.type canonical-lr
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

%%

program
    : program_item_list {
        TRACE("program");
        root_node = $$ = (ast_program_t*)create_ast_node(AST_PROGRAM);
        $$->program_item_list = $1;
    }
    ;

program_item_list
    : program_item {
        TRACE("program_item_list:CREATE");
        $$ = (ast_program_item_list_t*)create_ast_node(AST_PROGRAM_ITEM_LIST);
        $$->list = create_pointer_list();
        add_pointer_list($$->list, $1);
    }
    | program_item_list program_item {
        TRACE("program_item_list:ADD");
        add_pointer_list($1->list, $2);
    }
    ;

import_statement
    : IMPORT STRING_LIT {
        TRACE("import_statement: %s", $2->raw);
        // All processing happenes here, and not in an AST pass.
        $$ = (ast_import_statement_t*)create_ast_node(AST_IMPORT_STATEMENT);
        $$->STRING_LIT = $2;
        open_file($2->val.text);
    }
    ;

program_item
    : data_definition {
        TRACE("program_item:data_definition");
        $$ = (ast_program_item_t*)create_ast_node(AST_PROGRAM_ITEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | func_definition {
        TRACE("program_item:func_definition");
        $$ = (ast_program_item_t*)create_ast_node(AST_PROGRAM_ITEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | func_body {
        TRACE("program_item:func_body");
        $$ = (ast_program_item_t*)create_ast_node(AST_PROGRAM_ITEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | import_statement {
        TRACE("program_item:import_statement");
        $$ = (ast_program_item_t*)create_ast_node(AST_PROGRAM_ITEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | exception_identifier {
        TRACE("program_item:exception identifier");
        $$ = (ast_program_item_t*)create_ast_node(AST_PROGRAM_ITEM);
        $$->nterm = (ast_node_t*)$1;
    }
    ;

    /*
     * Create a exception handler ID symbol.
     */
exception_identifier
    : EXCEPT IDENTIFIER {
        TRACE("exception_identifier: %s", $2->raw);
        $$ = (ast_exception_identifier_t*)create_ast_node(AST_EXCEPTION_IDENTIFIER);
        $$->IDENTIFIER = $2;
    }
    ;

type_name
    : INTEGER {
        TRACE("type_name:INTEGER");
        $$ = (ast_type_name_t*)create_ast_node(AST_TYPE_NAME);
        $$->token = $1;
    }
    | FLOAT {
        TRACE("type_name:FLOAT");
        $$ = (ast_type_name_t*)create_ast_node(AST_TYPE_NAME);
        $$->token = $1;
    }
    | STRING {
        TRACE("type_name:STRING");
        $$ = (ast_type_name_t*)create_ast_node(AST_TYPE_NAME);
        $$->token = $1;
    }
    | LIST {
        TRACE("type_name:LIST");
        $$ = (ast_type_name_t*)create_ast_node(AST_TYPE_NAME);
        $$->token = $1;
    }
    | HASH {
        TRACE("type_name:HASH");
        $$ = (ast_type_name_t*)create_ast_node(AST_TYPE_NAME);
        $$->token = $1;
    }
    ;

formatted_string
    : STRING_LIT expression_list_param {
        TRACE("formatted_string:expression_list_param %s", $1->raw);
        $$ = (ast_formatted_string_t*)create_ast_node(AST_FORMATTED_STRING);
        $$->STRING_LIT = $1;
        $$->expression_list_param = $2;
    }
    | STRING_LIT {
        TRACE("formatted_string:nothing %s", $1->raw);
        $$ = (ast_formatted_string_t*)create_ast_node(AST_FORMATTED_STRING);
        $$->STRING_LIT = $1;
    }
    ;

    /*
     * Generate a data definition symbol.
     */
data_declaration
    : type_name IDENTIFIER {
        TRACE("data_declaration: %s", $2->raw);
        $$ = (ast_data_declaration_t*)create_ast_node(AST_DATA_DECLARATION);
        $$->type_name = $1;
        $$->IDENTIFIER = $2;
        $$->is_const = false;
    }
    | CONST type_name IDENTIFIER {
        TRACE("data_declaration: %s", $3->raw);
        $$ = (ast_data_declaration_t*)create_ast_node(AST_DATA_DECLARATION);
        $$->type_name = $2;
        $$->IDENTIFIER = $3;
        $$->is_const = true;
    }
    ;

data_declaration_list
    : data_declaration {
        TRACE("data_declaration_list:CREATE");
        $$ = (ast_data_declaration_list_t*)create_ast_node(AST_DATA_DECLARATION_LIST);
        $$->list = create_pointer_list();
        add_pointer_list($$->list, $1);
    }
    | data_declaration_list ',' data_declaration {
        TRACE("data_declaration_list:ADD");
        add_pointer_list($1->list, $3);
    }
    ;

data_definition
    : data_declaration {
        TRACE("data_definition: bare");
        $$ = (ast_data_definition_t*)create_ast_node(AST_DATA_DEFINITION);
        $$->data_declaration = $1;
        $$->is_init = false;
    }
    | data_declaration '=' expression {
        TRACE("data_definition: with expression");
        $$ = (ast_data_definition_t*)create_ast_node(AST_DATA_DEFINITION);
        $$->data_declaration = $1;
        $$->nterm = (ast_node_t*)$3;
        $$->is_init = true;
    }
    | data_declaration '=' list_init {
        TRACE("data_definition: with list_init");
        $$ = (ast_data_definition_t*)create_ast_node(AST_DATA_DEFINITION);
        $$->data_declaration = $1;
        $$->nterm = (ast_node_t*)$3;
        $$->is_init = true;
    }
    | data_declaration '=' dict_init {
        TRACE("data_definition: with dict_init");
        $$ = (ast_data_definition_t*)create_ast_node(AST_DATA_DEFINITION);
        $$->data_declaration = $1;
        $$->nterm = (ast_node_t*)$3;
        $$->is_init = true;
    }
    ;

    /*
     * Generate a function name symbol, but the decorated symbol name
     * cannot be generated until the parameters have been parsed.
     */
func_name
    : type_name IDENTIFIER {
        TRACE("func_name: type_name %s", $2->raw);
        $$ = (ast_func_name_t*)create_ast_node(AST_FUNC_NAME);
        $$->IDENTIFIER = $2;
        $$->type_name = $1;
        $$->is_iterator = false;

    }
    | ITERATOR type_name IDENTIFIER {
        TRACE("func_name: type_name %s", $3->raw);
        $$ = (ast_func_name_t*)create_ast_node(AST_FUNC_NAME);
        $$->IDENTIFIER = $3;
        $$->type_name = $2;
        $$->is_iterator = true;
    }
    | NOTHING IDENTIFIER {
        TRACE("func_name: NOTHING %s", $2->raw);
        $$ = (ast_func_name_t*)create_ast_node(AST_FUNC_NAME);
        $$->IDENTIFIER = $2;
        $$->is_iterator = false;
        ast_type_name_t* ptr = (ast_type_name_t*)create_ast_node(AST_TYPE_NAME);
        ptr->token = $1;
        $$->type_name = ptr;
    }
    ;

    /*
     * Add the parameters in their own symbol context and finish generating
     * the decorated function name.
     */
func_definition
    : func_name func_params func_body {
        TRACE("func_definition");
        $$ = (ast_func_definition_t*)create_ast_node(AST_FUNC_DEFINITION);
        $$->func_name = $1;
        $$->func_params = $2;
        $$->func_body = $3;
    }
    ;

func_params
    : '(' data_declaration_list ')' {
        TRACE("func_params: with decls");
        $$ = (ast_func_params_t*)create_ast_node(AST_FUNC_PARAMS);
        $$->data_declaration_list = $2;
    }
    | '(' ')' {
        TRACE("func_params: bare");
        $$ = (ast_func_params_t*)create_ast_node(AST_FUNC_PARAMS);
    }
    ;

func_body
    : '{' func_body_list '}' {
        TRACE("func_body: with body");
        $$ = (ast_func_body_t*)create_ast_node(AST_FUNC_BODY);
        $$->func_body_list = $2;
    }
    ;

func_body_list
    : func_body_elem {
        TRACE("func_body_list:CREATE");
        $$ = (ast_func_body_list_t*)create_ast_node(AST_FUNC_BODY_LIST);
        $$->list = create_pointer_list();
        add_pointer_list($$->list, $1);
    }
    | func_body_list func_body_elem {
        TRACE("func_body_list:ADD");
        add_pointer_list($1->list, $2);
    }
    ;

loop_body
    : '{' '}' {
        TRACE("loop_body: bare");
        $$ = (ast_loop_body_t*)create_ast_node(AST_LOOP_BODY);
    }
    | '{' loop_body_list '}' {
        TRACE("loop_body: with body");
        $$ = (ast_loop_body_t*)create_ast_node(AST_LOOP_BODY);
        $$->loop_body_list = $2;
    }
    ;

loop_body_list
    : loop_body_elem {
        TRACE("loop_body_list:CREATE");
        $$ = (ast_loop_body_list_t*)create_ast_node(AST_LOOP_BODY_LIST);
        $$->list = create_pointer_list();
        add_pointer_list($$->list, $1);
    }
    | loop_body_list loop_body_elem {
        TRACE("loop_body_list:ADD");
        add_pointer_list($1->list, $2);
    }
    ;

loop_body_diffs
    : BREAK {
        TRACE("loop_body_diffs:BREAK");
        $$ = (ast_loop_body_diffs_t*)create_ast_node(AST_LOOP_BODY_DIFFS);
        $$->type = BREAK;
    }
    | CONTINUE {
        TRACE("loop_body_diffs:CONTINUE");
        $$ = (ast_loop_body_diffs_t*)create_ast_node(AST_LOOP_BODY_DIFFS);
        $$->type = CONTINUE;
    }
    | YIELD '(' expression ')' {
        TRACE("loop_body_diffs:YIELD");
        $$ = (ast_loop_body_diffs_t*)create_ast_node(AST_LOOP_BODY_DIFFS);
        $$->type = YIELD;
        $$->expression = $3;
    }
    ;

loop_body_elem
    : func_body_elem {
        TRACE("loop_body_elem:func_body_elem");
        $$ = (ast_loop_body_elem_t*)create_ast_node(AST_LOOP_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | loop_body_diffs {
        TRACE("loop_body_elem:loop_body_diffs");
        $$ = (ast_loop_body_elem_t*)create_ast_node(AST_LOOP_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    ;

func_body_elem
    : data_definition {
        TRACE("func_body_elem:data_definition");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | func_reference {
        TRACE("func_body_elem:func_reference");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | assignment {
        TRACE("func_body_elem:assignment");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | while_statement {
        TRACE("func_body_elem:while_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | do_statement {
        TRACE("func_body_elem:do_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | for_statement {
        TRACE("func_body_elem:for_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | ifelse_statement {
        TRACE("func_body_elem:ifelse_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | tryexcept_statement {
        TRACE("func_body_elem:tryexcept_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | raise_statement {
        TRACE("func_body_elem:raise_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | return_statement {
        TRACE("func_body_elem:return_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | exit_statement {
        TRACE("func_body_elem:exit_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | print_statement {
        TRACE("func_body_elem:print_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | trace_statement {
        TRACE("func_body_elem:trace_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | inline_statement {
        TRACE("func_body_elem:inline_statement");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    | func_body {
        TRACE("func_body_elem:func_body");
        $$ = (ast_func_body_elem_t*)create_ast_node(AST_FUNC_BODY_ELEM);
        $$->nterm = (ast_node_t*)$1;
    }
    ;

inline_statement
    : INLINE {
        TRACE("trace_statement: with params");
        $$ = (ast_inline_statement_t*)create_ast_node(AST_INLINE_STATEMENT);
        $$->token = $1;
    }
    ;

trace_statement
    : TRACE expression_list_param {
        TRACE("trace_statement: with params");
        $$ = (ast_trace_statement_t*)create_ast_node(AST_TRACE_STATEMENT);
        $$->expression_list_param = $2;
    }
    | TRACE {
        TRACE("trace_statement: bare");
        $$ = (ast_trace_statement_t*)create_ast_node(AST_TRACE_STATEMENT);
    }
    ;

print_statement
    : PRINT expression_list_param {
        TRACE("print_statement: with params");
        $$ = (ast_print_statement_t*)create_ast_node(AST_PRINT_STATEMENT);
        $$->expression_list_param = $2;
    }
    | PRINT {
        TRACE("print_statement: bare");
        $$ = (ast_print_statement_t*)create_ast_node(AST_PRINT_STATEMENT);
    }
    ;

exit_statement
    : EXIT expression_param {
        TRACE("exit_statement: with params");
        $$ = (ast_exit_statement_t*)create_ast_node(AST_EXIT_STATEMENT);
        $$->expression_param = $2;
    }
    ;

return_statement
    : RETURN expression_param {
        TRACE("return_statement: with param");
        $$ = (ast_return_statement_t*)create_ast_node(AST_RETURN_STATEMENT);
        $$->expression_param = $2;
    }
    | RETURN {
        TRACE("return_statement: bare");
        $$ = (ast_return_statement_t*)create_ast_node(AST_RETURN_STATEMENT);
    }
    ;

    /*
     * If the type name is specified then this is a symbol definition, but
     * if not, then it is a symbol reference. Either way it gets it's own
     * symbol context similar to a function definition.
     */
for_statement
    : FOR '(' IDENTIFIER IN expression ')' func_body {
        TRACE("for_statement: no type spec");
        $$ = (ast_for_statement_t*)create_ast_node(AST_FOR_STATEMENT);
        $$->IDENTIFIER = $3;
        $$->expression = $5;
        $$->func_body = $7;
    }
    | FOR '(' type_name IDENTIFIER IN expression ')' func_body {
        TRACE("for_statement: with type spec");
        $$ = (ast_for_statement_t*)create_ast_node(AST_FOR_STATEMENT);
        $$->IDENTIFIER = $4;
        $$->expression = $6;
        $$->func_body = $8;
        $$->type_name = $3;
    }
    ;

tryexcept_statement
    : try_clause except_clause {
        TRACE("tryexcept_statement");
        $$ = (ast_tryexcept_statement_t*)create_ast_node(AST_TRYEXCEPT_STATEMENT);
        $$->try_clause = $1;
        $$->except_clause = $2;
    }
    ;

try_clause
    : TRY func_body {
        TRACE("try_clause");
        $$ = (ast_try_clause_t*)create_ast_node(AST_TRY_CLAUSE);
        $$->func_body = $2;
    }
    ;

    /*
     * The first identifier is a reference to an expression handler ID and the
     * second identifier is a symbol definition of a string that requires its
     * own symbol context, similar to a function definition.
     */
except_segment
    : EXCEPT '(' IDENTIFIER ',' IDENTIFIER ')' func_body {
        TRACE("except_segment(%s, %s)", $3->raw, $5->raw);
        $$ = (ast_except_segment_t*)create_ast_node(AST_EXCEPT_SEGMENT);
        $$->eident = $3;
        $$->mident = $5;
        $$->func_body = $7;
    }
    ;

except_clause_list
    : except_segment {
        TRACE("except_clause_list: CREATE");
        $$ = (ast_except_clause_list_t*)create_ast_node(AST_EXCEPT_CLAUSE_LIST);
        $$->list = create_pointer_list();
        add_pointer_list($$->list, $1);
    }
    | except_clause_list except_segment {
        TRACE("except_clause_list: ADD");
        add_pointer_list($1->list, $2);
    }
    ;

    /*
     * The identifier is a definition of a string symbol, same as the second
     * identifier in a except_segment.
     */
final_except_clause
    : EXCEPT '(' IDENTIFIER ')' func_body {
        TRACE("final_except_clause %s", $3->raw);
        $$ = (ast_final_except_clause_t*)create_ast_node(AST_FINAL_EXCEPT_CLAUSE);
        $$->IDENTIFIER = $3;
        $$->func_body = $5;
    }
    ;

except_clause
    : except_clause_list {
        TRACE("except_clause: alone");
        $$ = (ast_except_clause_t*)create_ast_node(AST_EXCEPT_CLAUSE);
        ($$)->except_clause_list = $1;
    }
    | except_clause_list final_except_clause {
        TRACE("except_clause: with final");
        $$ = (ast_except_clause_t*)create_ast_node(AST_EXCEPT_CLAUSE);
        $$->except_clause_list = $1;
        $$->final_except_clause = $2;
    }
    | final_except_clause {
        TRACE("except_clause:final_except_clause");
        $$ = (ast_except_clause_t*)create_ast_node(AST_EXCEPT_CLAUSE);
        $$->final_except_clause = $1;
    }
    ;

    /*
     * The identifier is a reference to an exception ID and the string is
     * assigned to the second identifier in an except clause.
     */
raise_statement
    : RAISE '(' IDENTIFIER ',' formatted_string ')' {
        TRACE("raise_statement %s", $3->raw);
        $$ = (ast_raise_statement_t*)create_ast_node(AST_RAISE_STATEMENT);
        $$->IDENTIFIER = $3;
        $$->formatted_string = $5;
    }
    ;

if_clause
    : IF '(' expression ')' func_body {
        TRACE("if_clause");
        $$ = (ast_if_clause_t*)create_ast_node(AST_IF_CLAUSE);
        $$->expression = $3;
        $$->func_body = $5;
    }
    ;

ifelse_statement
    : if_clause {
        TRACE("ifelse_statement: bare");
        $$ = (ast_ifelse_statement_t*)create_ast_node(AST_IFELSE_STATEMENT);
        $$->if_clause = $1;
    }
    | if_clause else_clause {
        TRACE("ifelse_statement: with else");
        $$ = (ast_ifelse_statement_t*)create_ast_node(AST_IFELSE_STATEMENT);
        $$->if_clause = $1;
        $$->else_clause = $2;
    }
    ;

else_segment
    : ELSE '(' expression ')' func_body {
        TRACE("else_segment");
        $$ = (ast_else_segment_t*)create_ast_node(AST_ELSE_SEGMENT);
        $$->expression = $3;
        $$->func_body = $5;
    }
    ;

final_else_segment
    : ELSE '(' ')' func_body {
        TRACE("final_else_segment: with parens");
        $$ = (ast_final_else_segment_t*)create_ast_node(AST_FINAL_ELSE_SEGMENT);
        $$->func_body = $4;
    }
    | ELSE func_body {
        TRACE("final_else_segment: bare");
        $$ = (ast_final_else_segment_t*)create_ast_node(AST_FINAL_ELSE_SEGMENT);
        $$->func_body = $2;
    }
    ;

else_clause_list
    : else_segment {
        TRACE("else_clause_list:CREATE");
        $$ = (ast_else_clause_list_t*)create_ast_node(AST_ELSE_CLAUSE_LIST);
        $$->list = create_pointer_list();
        add_pointer_list($$->list, $1);
    }
    | else_clause_list else_segment {
        TRACE("else_clause_list:ADD");
        add_pointer_list($1->list, $2);
    }
    ;

else_clause
    : else_clause_list {
        TRACE("else_clause: bare");
        $$ = (ast_else_clause_t*)create_ast_node(AST_ELSE_CLAUSE);
        $$->else_clause_list = $1;
    }
    | else_clause_list final_else_segment {
        TRACE("else_clause: with final");
        $$ = (ast_else_clause_t*)create_ast_node(AST_ELSE_CLAUSE);
        $$->else_clause_list = $1;
        $$->final_else_segment = $2;
    }
    | final_else_segment {
        TRACE("else_clause:final_else_segment");
        $$ = (ast_else_clause_t*)create_ast_node(AST_ELSE_CLAUSE);
        $$->final_else_segment = $1;
    }
    ;

while_clause
    : WHILE expression_param {
        TRACE("while_clause: with expr");
        $$ = (ast_while_clause_t*)create_ast_node(AST_WHILE_CLAUSE);
        $$->expression_param = $2;
    }
    | WHILE {
        TRACE("while_clause: bare");
        $$ = (ast_while_clause_t*)create_ast_node(AST_WHILE_CLAUSE);
    }
    ;

while_statement
    : while_clause loop_body {
        TRACE("while_statement");
        $$ = (ast_while_statement_t*)create_ast_node(AST_WHILE_STATEMENT);
        $$->loop_body = $2;
        $$->while_clause = $1;
    }
    ;

do_statement
    : DO loop_body while_clause {
        TRACE("do_statement");
        $$ = (ast_do_statement_t*)create_ast_node(AST_DO_STATEMENT);
        $$->loop_body = $2;
        $$->while_clause = $3;
    }
    ;

assignment_right
    : expression {
        TRACE("assignment_right");
        $$ = (ast_assignment_right_t*)create_ast_node(AST_ASSIGNMENT_RIGHT);
        $$->nterm = (ast_node_t*)$1;
    }
    | list_init {
        TRACE("assignment_right:list_init");
        $$ = (ast_assignment_right_t*)create_ast_node(AST_ASSIGNMENT_RIGHT);
        $$->nterm = (ast_node_t*)$1;
    }
    | dict_init {
        TRACE("assignment_right:dict_init");
        $$ = (ast_assignment_right_t*)create_ast_node(AST_ASSIGNMENT_RIGHT);
        $$->nterm = (ast_node_t*)$1;
    }
    | TYPE ':' expression {
        $$ = (ast_assignment_right_t*)create_ast_node(AST_ASSIGNMENT_RIGHT);
        $$->nterm = (ast_node_t*)$3;
    }
    | type_name ':' expression {
        TRACE("assignment_right: as a cast");
        $$ = (ast_assignment_right_t*)create_ast_node(AST_ASSIGNMENT_RIGHT);
        $$->nterm = (ast_node_t*)$3;
        $$->type_name = $1;
    }
    ;

list_init
    : '[' expression_list ']' {
        TRACE("list_init");
        $$ = (ast_list_init_t*)create_ast_node(AST_LIST_INIT);
        $$->expression_list = $2;
    }
    ;

dict_init_item
    : STRING_LIT ':' expression {
        TRACE("dict_init_item");
        $$ = (ast_dict_init_item_t*)create_ast_node(AST_DICT_INIT_ITEM);
        $$->STRING_LIT = $1;
        $$->expression = $3;
    }
    ;

dict_init_item_list
    : dict_init_item {
        TRACE("dict_init_item_list: CREATE");
        $$ = (ast_dict_init_item_list_t*)create_ast_node(AST_DICT_INIT_ITEM_LIST);
        $$->list = create_pointer_list();
        add_pointer_list($$->list, $1);
    }
    | dict_init_item_list ',' dict_init_item {
        TRACE("dict_init_item_list: ADD");
        add_pointer_list($1->list, $3);
    }
    ;

dict_init
    : '[' dict_init_item_list ']' {
        TRACE("dict_init");
        $$ = (ast_dict_init_t*)create_ast_node(AST_DICT_INIT);
        $$->dict_init_item_list = $2;
    }
    ;

    /*
     * Identifier is a reference that has to be examined when the variable
     * references are verified.
     */
assignment_left
    : IDENTIFIER {
        TRACE("assignment_left: identifier %s", $1->raw);
        $$ = (ast_assignment_left_t*)create_ast_node(AST_ASSIGNMENT_LEFT);
        $$->IDENTIFIER = $1;
    }
    | list_reference {
        TRACE("assignment_left: list");
        $$ = (ast_assignment_left_t*)create_ast_node(AST_ASSIGNMENT_LEFT);
        $$->list_reference = $1;
    }
    ;

assignment
    : assignment_left '=' assignment_right {
        TRACE("assignment");
        $$ = (ast_assignment_t*)create_ast_node(AST_ASSIGNMENT);
        $$->assignment_left = $1;
        $$->assignment_right = $3;
    }
    ;

    /*
     * references only.
     */
raw_value
    : IDENTIFIER {
        TRACE("raw_value:IDENTIFIER %s", $1->raw);
        $$ = (ast_raw_value_t*)create_ast_node(AST_RAW_VALUE);
        $$->token = $1;
    }
    | INTEGER_LIT {
        TRACE("raw_value:INTEGER_LIT %ld", $1->val.integer_lit);
        $$ = (ast_raw_value_t*)create_ast_node(AST_RAW_VALUE);
        $$->token = $1;
    }
    | FLOAT_LIT {
        TRACE("raw_value:FLOAT_LIT %lf", $1->val.float_lit);
        $$ = (ast_raw_value_t*)create_ast_node(AST_RAW_VALUE);
        $$->token = $1;
    }
    ;

expr_primary
    : raw_value {
        TRACE("expr_primary:raw_value");
        $$ = (ast_expr_primary_t*)create_ast_node(AST_EXPR_PRIMARY);
        $$->nterm = (ast_node_t*)$1;
    }
    | formatted_string {
        TRACE("expr_primary:formatted_string");
        $$ = (ast_expr_primary_t*)create_ast_node(AST_EXPR_PRIMARY);
        $$->nterm = (ast_node_t*)$1;
    }
    | list_reference {
        TRACE("expr_primary:list_reference");
        $$ = (ast_expr_primary_t*)create_ast_node(AST_EXPR_PRIMARY);
        $$->nterm = (ast_node_t*)$1;
    }
    | func_reference {
        TRACE("expr_primary:func_reference");
        $$ = (ast_expr_primary_t*)create_ast_node(AST_EXPR_PRIMARY);
        $$->nterm = (ast_node_t*)$1;
    }
    ;

    /*
     * This includes hashes as well as lists, since they are the same syntax.
     */
list_reference
    : IDENTIFIER list_ref_param_list {
        TRACE("list_reference: %s", $1->raw);
        $$ = (ast_list_reference_t*)create_ast_node(AST_LIST_REFERENCE);
        $$->IDENTIFIER = $1;
        $$->list_ref_param_list = $2;
    }
    ;

    /*
     * These have to be throurally check for symantic correctness.
     */
list_ref_value
    : IDENTIFIER {
        TRACE("list_ref_value:IDENTIFIER");
        $$ = (ast_list_ref_value_t*)create_ast_node(AST_LIST_REF_VALUE);
        $$->token = $1;
    }
    | INTEGER_LIT {
        TRACE("list_ref_value:INTEGER_LIT");
        $$ = (ast_list_ref_value_t*)create_ast_node(AST_LIST_REF_VALUE);
        $$->token = $1;
    }
    | STRING_LIT {
        TRACE("list_ref_value:STRING_LIT");
        $$ = (ast_list_ref_value_t*)create_ast_node(AST_LIST_REF_VALUE);
        $$->token = $1;
    }
    | list_reference {
        TRACE("list_ref_value:list_reference");
        $$ = (ast_list_ref_value_t*)create_ast_node(AST_LIST_REF_VALUE);
        $$->nterm = (ast_node_t*)$1;
    }
    | func_reference {
        TRACE("list_ref_value:func_reference");
        $$ = (ast_list_ref_value_t*)create_ast_node(AST_LIST_REF_VALUE);
        $$->nterm = (ast_node_t*)$1;
    }
    | list_ref_param {
        TRACE("list_ref_value:list_ref_param");
        $$ = (ast_list_ref_value_t*)create_ast_node(AST_LIST_REF_VALUE);
        $$->nterm = (ast_node_t*)$1;
    }
    ;

list_ref_param
    : '[' list_ref_value ']' {
        TRACE("list_ref_param");
        $$ = (ast_list_ref_param_t*)create_ast_node(AST_LIST_REF_PARAM);
        $$->list_ref_value = $2;
    }
    ;

list_ref_param_list
    : list_ref_param {
        TRACE("list_ref_param_list: CREATE");
        $$ = (ast_list_ref_param_list_t*)create_ast_node(AST_LIST_REF_PARAM_LIST);
        $$->list = create_pointer_list();
        add_pointer_list($$->list, $1);
    }
    | list_ref_param_list list_ref_param {
        TRACE("list_ref_param_list: ADD");
        add_pointer_list($1->list, $2);
    }
    ;

func_reference
    : IDENTIFIER expression_list_param {
        TRACE("func_reference %s", $1->raw);
        $$ = (ast_func_reference_t*)create_ast_node(AST_FUNC_REFERENCE);
        $$->IDENTIFIER = $1;
        $$->expression_list_param = $2;
    }
    ;

expression
    : expr_primary {
        TRACE("expression:expr_primary");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->expr_primary = $1;
    }
    | expression ADD_OPER expression {
        TRACE("expression:+");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression SUB_OPER expression {
        TRACE("expression:-");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression MUL_OPER expression {
        TRACE("expression:*");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression DIV_OPER expression {
        TRACE("expression:/");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression MOD_OPER expression {
        TRACE("expression:%%");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression POW_OPER expression {
        TRACE("expression:^");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression GT_OPER expression {
        TRACE("expression:GT_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression LT_OPER expression {
        TRACE("expression:LT_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression EQU_OPER expression {
        TRACE("expression:EQU_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression NEQ_OPER expression {
        TRACE("expression:NEQ_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression LTE_OPER expression {
        TRACE("expression:LTE_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression GTE_OPER expression {
        TRACE("expression:GTE_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression OR_OPER expression {
        TRACE("expression:OR_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | expression AND_OPER expression {
        TRACE("expression:AND_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->left = $1;
        $$->right = $3;
        $$->oper = $2;
    }
    | NOT_OPER expression %prec UNARY {
        TRACE("expression:unary NOT_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->right = $2;
        $$->oper = $1;
    }
    | SUB_OPER expression %prec UNARY {
        TRACE("expression:unary SUB_OPER");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->right = $2;
        $$->oper = $1;
    }
    | '(' expression ')' {
        TRACE("expr_primary:(expression)");
        $$ = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
        $$->expression = $2;
    }
    ;

expression_list
    : expression {
        TRACE("expression_list: CREATE");
        $$ = (ast_expression_list_t*)create_ast_node(AST_EXPRESSION_LIST);
        $$->list = create_pointer_list();
        add_pointer_list($$->list, $1);
    }
    | expression_list ',' expression {
        TRACE("expression_list: ADD");
        add_pointer_list($1->list, $3);
    }
    ;

expression_list_param
    : '(' expression_list ')' {
        TRACE("expression_list_param: with params");
        $$ = (ast_expression_list_param_t*)create_ast_node(AST_EXPRESSION_LIST_PARAM);
        $$->expression_list = $2;
    }
    | '(' ')' {
        TRACE("expression_list_param: bare");
        $$ = (ast_expression_list_param_t*)create_ast_node(AST_EXPRESSION_LIST_PARAM);
    }
    ;

expression_param
    : '(' expression ')' {
        TRACE("expression_param: with expression");
        $$ = (ast_expression_param_t*)create_ast_node(AST_EXPRESSION_PARAM);
        $$->expression = $2;
    }
    | '(' ')' {
        TRACE("expression_param: bare");
        $$ = (ast_expression_param_t*)create_ast_node(AST_EXPRESSION_PARAM);
    }
    ;

%%

void yyerror(const char* s) {

    //parser_syntax_error("%s", s);
    fprintf(stderr, "%s:%d:%d: %s\n", get_file_name(), get_line_no(), get_col_no(), s);

}

const char* token_to_str(int tok) {

    return yysymbol_name(YYTRANSLATE(tok));
}
