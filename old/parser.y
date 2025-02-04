%{
#include <stdio.h>
#include <stdint.h>

#include "scanner.h"
#include "memory.h"
#include "fileio.h"
#include "errors.h"
#include "ast.h"
#include "symtab.h"

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
extern ast_node_t* root_node;

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
};

%token <token> IDENTIFIER STRING_LIT INTEGER_LIT FLOAT_LIT
%token <token> INTEGER FLOAT STRING LIST HASH NOTHING
%token <token> EQU_OPER NEQ_OPER LTE_OPER GTE_OPER LT_OPER
%token <token> NOT_OPER OR_OPER AND_OPER GT_OPER UNARY_MINUS_OPER
%token <token> ADD_OPER SUB_OPER MUL_OPER DIV_OPER MOD_OPER POW_OPER

%token IF ELSE WHILE DO TRY EXCEPT RAISE IMPORT YIELD FOR IN
%token BREAK CONTINUE RETURN PRINT TRACE EXIT CONST ITERATOR

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

    /*
     * nodes that generate a symbol.
     */
%nterm <node> data_declaration exception_identifier

    // %define lr.type canonical-lr
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
    : program_item_list {
        TRACE("program");
        root_node = $$ = create_ast_node(AST_PROGRAM);
        add_ast_node_attrib($$, "program_item_list", $1);
    }
    ;

program_item_list
    : program_item {
        TRACE("program_item_list:CREATE");
        $$ = create_ast_node(AST_PROGRAM_ITEM_LIST);
        add_ast_node_list_item($$, "list", $1);
    }
    | program_item_list program_item {
        TRACE("program_item_list:ADD");
        add_ast_node_list_item($1, "list", $2);
    }
    ;

import_statement
    : IMPORT STRING_LIT {
        TRACE("program_item:import_statement");
        // All processing happenes here, and not in an AST pass.
        open_file($2->val.text);
    }
    ;

program_item
    : data_definition {
        TRACE("program_item:data_definition");
        $$ = create_ast_node(AST_PROGRAM_ITEM);
        ast_type_t type = AST_DATA_DEFINITION;
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
        add_ast_node_attrib($$, "node", $1);
    }
    | func_definition {
        TRACE("program_item:func_definition");
        $$ = create_ast_node(AST_PROGRAM_ITEM);
        ast_type_t type = AST_FUNC_DEFINITION;
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
        add_ast_node_attrib($$, "node", $1);
    }
    | func_body {
        TRACE("program_item:func_body");
        $$ = create_ast_node(AST_PROGRAM_ITEM);
        ast_type_t type = AST_FUNC_BODY;
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
        add_ast_node_attrib($$, "node", $1);
    }
    | import_statement {
        TRACE("program_item:import_statement");
        $$ = create_ast_node(AST_PROGRAM_ITEM);
        ast_type_t type = AST_IMPORT_STATEMENT;
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
        add_ast_node_attrib($$, "node", $1);
    }
    | exception_identifier {
        TRACE("program_item:exception identifier");
        $$ = create_ast_node(AST_PROGRAM_ITEM);
        ast_type_t type = AST_EXCEPT_ID;
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
        add_ast_node_attrib($$, "node", $1);
    }
    ;

    /*
     * Create a exception handler ID symbol.
     */
exception_identifier
    : EXCEPT IDENTIFIER {
        TRACE("exception_identifier: %s", $2);
        $$ = create_ast_node(AST_EXCEPT_ID);
        //add_ast_node_attrib($$, "token", $2);
        symbol_t* sym = create_symbol(SYM_EXCEPT, $2);
        sym->sym_type = EXCEPT;
        sym->is_const = true;
        sym->is_init = true; // init by the system
        add_ast_node_attrib($$, "symbol", sym);
    }
    ;

type_name
    : INTEGER {
        TRACE("type_name:INTEGER");
        $$ = create_ast_node(AST_TYPE_NAME);
        add_ast_node_attrib($$, "token", $1);
    }
    | FLOAT {
        TRACE("type_name:FLOAT");
        $$ = create_ast_node(AST_TYPE_NAME);
        add_ast_node_attrib($$, "token", $1);
    }
    | STRING {
        TRACE("type_name:STRING");
        $$ = create_ast_node(AST_TYPE_NAME);
        add_ast_node_attrib($$, "token", $1);
    }
    | LIST {
        TRACE("type_name:LIST");
        $$ = create_ast_node(AST_TYPE_NAME);
        add_ast_node_attrib($$, "token", $1);
    }
    | HASH {
        TRACE("type_name:HASH");
        $$ = create_ast_node(AST_TYPE_NAME);
        add_ast_node_attrib($$, "token", $1);
    }
    ;

formatted_string
    : STRING_LIT expression_list_param {
        TRACE("formatted_string:expression_list_param %s", $1);
        $$ = create_ast_node(AST_FORMATTED_STRING);
        add_ast_node_attrib($$, "expression_list_param",  $2);
        add_ast_node_attrib($$, "token", $1);
    }
    | STRING_LIT {
        TRACE("formatted_string:nothing %s", $1);
        $$ = create_ast_node(AST_FORMATTED_STRING);
        add_ast_node_attrib($$, "token",  $1);
    }
    ;

    /*
     * Generate a data definition symbol.
     */
data_declaration
    : type_name IDENTIFIER {
        TRACE("data_declaration: %s", $2);
        $$ = create_ast_node(AST_DATA_DECLARATION);
        symbol_t* sym = create_symbol(SYM_DATA, $2);
        sym->name = ((token_t*)$2)->raw;
        void* ptr;
        find_hashtable(((ast_node_t*)$1)->table, "token", &ptr);
        sym->sym_type = ((token_t*)ptr)->type;
        add_ast_node_attrib($$, "symbol", sym);

//         add_ast_node_attrib($$, "type_name", $1);
//         add_ast_node_attrib($$, "token", $2);
//         int flag = 0;
//         add_ast_node_attrib($$, "is_const", _COPY_DS(&flag, int));
    }
    | CONST type_name IDENTIFIER {
        TRACE("data_declaration: %s", $3);
        $$ = create_ast_node(AST_DATA_DECLARATION);
        add_ast_node_attrib($$, "type_name", $2);
        add_ast_node_attrib($$, "token", $3);
        int flag = 1;
        add_ast_node_attrib($$, "is_const", _COPY_DS(&flag, int));
    }
    ;

data_declaration_list
    : data_declaration {
        TRACE("data_declaration_list:CREATE");
        $$ = create_ast_node(AST_DATA_DECLARATION_LIST);
        add_ast_node_list_item($$, "list", $1);
    }
    | data_declaration_list ',' data_declaration {
        TRACE("data_declaration_list:ADD");
        add_ast_node_list_item($1, "list", $3);
    }
    ;

data_definition
    : data_declaration {
        TRACE("data_definition: bare");
        int flag = 0;
        int type = 0;
        $$ = create_ast_node(AST_DATA_DEFINITION);
        add_ast_node_attrib($$, "data_declaration", $1);
        add_ast_node_attrib($$, "is_init", _COPY_DS(&flag, int));
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | data_declaration '=' expression {
        TRACE("data_definition: with expression");
        int flag = 1;
        int type = 1;
        $$ = create_ast_node(AST_DATA_DEFINITION);
        add_ast_node_attrib($$, "data_declaration", $1);
        add_ast_node_attrib($$, "expression", $3);
        add_ast_node_attrib($$, "is_init", _COPY_DS(&flag, int));
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | data_declaration '=' list_init {
        int type = 2;
        int flag = 1;
        $$ = create_ast_node(AST_DATA_DEFINITION);
        add_ast_node_attrib($$, "data_declaration", $1);
        add_ast_node_attrib($$, "list_init", $3);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
        add_ast_node_attrib($$, "is_init", _COPY_DS(&flag, int));
    }
    | data_declaration '=' dict_init {
        int type = 3;
        int flag = 1;
        $$ = create_ast_node(AST_DATA_DEFINITION);
        add_ast_node_attrib($$, "data_declaration", $1);
        add_ast_node_attrib($$, "dict_init", $3);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
        add_ast_node_attrib($$, "is_init", _COPY_DS(&flag, int));
    }
    ;

    /*
     * Generate a function name symbol, but the decorated symbol name
     * cannot be generated until the parameters have been parsed.
     */
func_name
    : type_name IDENTIFIER {
        TRACE("func_name: type_name %s", $2);
        $$ = create_ast_node(AST_FUNC_NAME);
        add_ast_node_attrib($$, "token", $2);
        add_ast_node_attrib($$, "type_name", $1);
        int flag = 0;
        add_ast_node_attrib($$, "is_iter", _COPY_DS(&flag, int));
    }
    | ITERATOR type_name IDENTIFIER {
        TRACE("func_name: type_name %s", $2);
        $$ = create_ast_node(AST_FUNC_NAME);
        add_ast_node_attrib($$, "token", $3);
        add_ast_node_attrib($$, "type_name", $2);
        int flag = 1;
        add_ast_node_attrib($$, "is_iter", _COPY_DS(&flag, int));
    }
    | NOTHING IDENTIFIER {
        TRACE("func_name: NOTHING %s", $2);
        $$ = create_ast_node(AST_FUNC_NAME);
        add_ast_node_attrib($$, "token", $2);
        ast_node_t* type = create_ast_node(AST_TYPE_NAME);
        add_ast_node_attrib(type, "token", $1);
        add_ast_node_attrib($$, "type_name", type);
        int flag = 0;
        add_ast_node_attrib($$, "is_iter", _COPY_DS(&flag, int));
    }
    ;

    /*
     * Add the parameters in their own symbol context and finish generating
     * the decorated function name.
     */
func_definition
    : func_name func_params func_body {
        TRACE("func_definition");
        $$ = create_ast_node(AST_FUNC_DEFINITION);
        add_ast_node_attrib($$, "func_name", $1);
        add_ast_node_attrib($$, "func_params", $2);
        add_ast_node_attrib($$, "func_body", $3);
    }
    ;

func_params
    : '(' data_declaration_list ')' {
        TRACE("func_params: with decls");
        $$ = create_ast_node(AST_FUNC_PARAMS);
        add_ast_node_attrib($$, "data_declaration_list", $2);
    }
    | '(' ')' {
        TRACE("func_params: bare");
        $$ = create_ast_node(AST_FUNC_PARAMS);
        //add_ast_node_attrib($$, "data_declaration_list", NULL);
    }
    ;

func_body
    : '{' func_body_list '}' {
        TRACE("func_body: with body");
        $$ = create_ast_node(AST_FUNC_BODY);
        add_ast_node_attrib($$, "func_body_list", $2);
    }
    ;

func_body_list
    : func_body_elem {
        TRACE("func_body_list:CREATE");
        $$ = create_ast_node(AST_FUNC_BODY_LIST);
        add_ast_node_list_item($$, "list", $1);
    }
    | func_body_list func_body_elem {
        TRACE("func_body_list:ADD");
        add_ast_node_list_item($1, "list", $2);
    }
    ;

loop_body
    : '{' '}' {
        TRACE("loop_body: bare");
        $$ = create_ast_node(AST_LOOP_BODY);
        add_ast_node_attrib($$, "loop_body_list", NULL);
    }
    | '{' loop_body_list '}' {
        TRACE("loop_body: with body");
        $$ = create_ast_node(AST_LOOP_BODY);
        add_ast_node_attrib($$, "loop_body_list", $2);
    }
    ;

loop_body_list
    : loop_body_elem {
        TRACE("loop_body_list:CREATE");
        $$ = create_ast_node(AST_LOOP_BODY_LIST);
        add_ast_node_list_item($$, "list", $1);
    }
    | loop_body_list loop_body_elem {
        TRACE("loop_body_list:ADD");
        add_ast_node_list_item($1, "list", $2);
    }
    ;

loop_body_diffs
    : BREAK {
        TRACE("loop_body_diffs:BREAK");
        int type = BREAK;
        $$ = create_ast_node(AST_LOOP_BODY_DIFFS);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | CONTINUE {
        TRACE("loop_body_diffs:CONTINUE");
        int type = CONTINUE;
        $$ = create_ast_node(AST_LOOP_BODY_DIFFS);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | YIELD '(' expression ')' {
        TRACE("loop_body_diffs:YIELD");
        int type = YIELD;
        $$ = create_ast_node(AST_LOOP_BODY_DIFFS);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
        add_ast_node_attrib($$, "expression", $3);
    }
    ;

loop_body_elem
    : func_body_elem {
        TRACE("loop_body_elem:func_body_elem");
        ast_type_t type = AST_FUNC_BODY_ELEM;
        $$ = create_ast_node(AST_LOOP_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | loop_body_diffs {
        TRACE("loop_body_elem:loop_body_diffs");
        ast_type_t type = AST_LOOP_BODY_DIFFS;
        $$ = create_ast_node(AST_LOOP_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    ;

func_body_elem
    : data_definition {
        TRACE("func_body_elem:data_definition");
        ast_type_t type = AST_DATA_DEFINITION;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | func_reference {
        TRACE("func_body_elem:func_reference");
        ast_type_t type = AST_FUNC_REFERENCE;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | assignment {
        TRACE("func_body_elem:assignment");
        ast_type_t type = AST_ASSIGNMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | while_statement {
        TRACE("func_body_elem:while_statement");
        ast_type_t type = AST_WHILE_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | do_statement {
        TRACE("func_body_elem:do_statement");
        ast_type_t type = AST_DO_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | for_statement {
        TRACE("func_body_elem:for_statement");
        ast_type_t type = AST_FOR_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | ifelse_statement {
        TRACE("func_body_elem:ifelse_statement");
        ast_type_t type = AST_IFELSE_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | tryexcept_statement {
        TRACE("func_body_elem:tryexcept_statement");
        ast_type_t type = AST_TRYEXCEPT_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | raise_statement {
        TRACE("func_body_elem:raise_statement");
        ast_type_t type = AST_RAISE_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | return_statement {
        TRACE("func_body_elem:return_statement");
        ast_type_t type = AST_RETURN_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | exit_statement {
        TRACE("func_body_elem:exit_statement");
        ast_type_t type = AST_EXIT_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | print_statement {
        TRACE("func_body_elem:print_statement");
        ast_type_t type = AST_PRINT_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | trace_statement {
        TRACE("func_body_elem:trace_statement");
        ast_type_t type = AST_TRACE_STATEMENT;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | func_body {
        TRACE("func_body_elem:func_body");
        ast_type_t type = AST_FUNC_BODY;
        $$ = create_ast_node(AST_FUNC_BODY_ELEM);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    ;

trace_statement
    : TRACE expression_list_param {
        TRACE("trace_statement: with params");
        $$ = create_ast_node(AST_TRACE_STATEMENT);
        add_ast_node_attrib($$, "expression_list_param", $2);
    }
    | TRACE {
        TRACE("trace_statement: bare");
        $$ = create_ast_node(AST_TRACE_STATEMENT);
        //add_ast_node_attrib($$, "expression_list_param", NULL);
    }
    ;

print_statement
    : PRINT expression_list_param {
        TRACE("print_statement: with params");
        $$ = create_ast_node(AST_PRINT_STATEMENT);
        add_ast_node_attrib($$, "expression_list_param", $2);
    }
    | PRINT {
        TRACE("print_statement: bare");
        $$ = create_ast_node(AST_PRINT_STATEMENT);
        //add_ast_node_attrib($$, "expression_list_param", NULL);
    }
    ;

exit_statement
    : EXIT expression_param {
        TRACE("exit_statement: with params");
        $$ = create_ast_node(AST_EXIT_STATEMENT);
        add_ast_node_attrib($$, "expression_param", $2);
    }
    ;

return_statement
    : RETURN expression_param {
        TRACE("return_statement: with param");
        $$ = create_ast_node(AST_RETURN_STATEMENT);
        add_ast_node_attrib($$, "expression_param", $2);
    }
    | RETURN {
        TRACE("return_statement: bare");
        $$ = create_ast_node(AST_RETURN_STATEMENT);
        //add_ast_node_attrib($$, "expression_param", NULL);
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
        $$ = create_ast_node(AST_FOR_STATEMENT);
        add_ast_node_attrib($$, "token", $3);
        add_ast_node_attrib($$, "expression", $5);
        add_ast_node_attrib($$, "func_body", $7);
    }
    | FOR '(' type_name IDENTIFIER IN expression ')' func_body {
        TRACE("for_statement: with type spec");
        $$ = create_ast_node(AST_FOR_STATEMENT);
        add_ast_node_attrib($$, "token", $4);
        add_ast_node_attrib($$, "expression", $6);
        add_ast_node_attrib($$, "func_body", $8);
        add_ast_node_attrib($$, "type_name", $3);
    }
    ;

tryexcept_statement
    : try_clause except_clause {
        TRACE("tryexcept_statement");
        $$ = create_ast_node(AST_TRYEXCEPT_STATEMENT);
        add_ast_node_attrib($$, "except_clause", $2);
        add_ast_node_attrib($$, "try_clause", $1);
    }
    ;

try_clause
    : TRY func_body {
        TRACE("try_clause");
        $$ = create_ast_node(AST_TRY_CLAUSE);
        add_ast_node_attrib($$, "func_body", $2);
    }
    ;

    /*
     * The first identifier is a reference to an expression handler ID and the
     * second identifier is a symbol definition of a string that requires its
     * own symbol context, similar to a function definition.
     */
except_segment
    : EXCEPT '(' IDENTIFIER ',' IDENTIFIER ')' func_body {
        TRACE("except_segment(%s, %s)", $3, $5);
        $$ = create_ast_node(AST_EXCEPT_SEGMENT);
        add_ast_node_attrib($$, "func_body", $7);
        add_ast_node_attrib($$, "ename", $3);
        add_ast_node_attrib($$, "mname", $5);
    }
    ;

except_clause_list
    : except_segment {
        TRACE("except_clause_list: CREATE");
        $$ = create_ast_node(AST_EXCEPT_CLAUSE_LIST);
        add_ast_node_list_item($$, "list", $1);
    }
    | except_clause_list except_segment {
        TRACE("except_clause_list: ADD");
        add_ast_node_list_item($1, "list", $2);
    }
    ;

    /*
     * The identifier is a definition of a string symbol, same as the second
     * identifier in a except_segment.
     */
final_except_clause
    : EXCEPT '(' IDENTIFIER ')' func_body {
        TRACE("final_except_clause %s", $3);
        $$ = create_ast_node(AST_FINAL_EXCEPT_CLAUSE);
        add_ast_node_attrib($$, "func_body", $5);
        add_ast_node_attrib($$, "mname", $3);
    }
    ;

except_clause
    : except_clause_list {
        TRACE("except_clause: alone");
        $$ = create_ast_node(AST_EXCEPT_CLAUSE);
        add_ast_node_attrib($$, "except_clause_list", $1);
    }
    | except_clause_list final_except_clause {
        TRACE("except_clause: with final");
        $$ = create_ast_node(AST_EXCEPT_CLAUSE);
        add_ast_node_attrib($$, "except_clause_list", $1);
        add_ast_node_attrib($$, "final_except_clause", $2);
    }
    | final_except_clause {
        TRACE("except_clause:final_except_clause");
        $$ = create_ast_node(AST_EXCEPT_CLAUSE);
        add_ast_node_attrib($$, "final_except_clause", $1);
    }
    ;

    /*
     * The identifier is a reference to an exception ID and the string is
     * assigned to the second identifier in an except clause.
     */
raise_statement
    : RAISE '(' IDENTIFIER ',' formatted_string ')' {
        TRACE("raise_statement %s", $3);
        $$ = create_ast_node(AST_RAISE_STATEMENT);
        add_ast_node_attrib($$, "formatted_string", $5);
        add_ast_node_attrib($$, "ename", $3);
    }
    ;

if_clause
    : IF '(' expression ')' func_body {
        TRACE("if_clause");
        $$ = create_ast_node(AST_IF_CLAUSE);
        add_ast_node_attrib($$, "func_body", $5);
        add_ast_node_attrib($$, "expression", $3);
    }
    ;

ifelse_statement
    : if_clause {
        TRACE("ifelse_statement: bare");
        $$ = create_ast_node(AST_IFELSE_STATEMENT);
        add_ast_node_attrib($$, "if_clause", $1);
    }
    | if_clause else_clause {
        TRACE("ifelse_statement: with else");
        $$ = create_ast_node(AST_IFELSE_STATEMENT);
        add_ast_node_attrib($$, "if_clause", $1);
        add_ast_node_attrib($$, "else_clause", $2);
    }
    ;

else_segment
    : ELSE '(' expression ')' func_body {
        TRACE("else_segment");
        $$ = create_ast_node(AST_ELSE_SEGMENT);
        add_ast_node_attrib($$, "func_body", $5);
        add_ast_node_attrib($$, "expression", $3);
    }
    ;

final_else_segment
    : ELSE '(' ')' func_body {
        TRACE("final_else_segment: with parens");
        $$ = create_ast_node(AST_FINAL_ELSE_SEGMENT);
        add_ast_node_attrib($$, "func_body", $4);
    }
    | ELSE func_body {
        TRACE("final_else_segment: bare");
        $$ = create_ast_node(AST_FINAL_ELSE_SEGMENT);
        add_ast_node_attrib($$, "func_body", $2);
    }
    ;

else_clause_list
    : else_segment {
        TRACE("else_clause_list:CREATE");
        $$ = create_ast_node(AST_ELSE_CLAUSE_LIST);
        add_ast_node_list_item($$, "list", $1);
    }
    | else_clause_list else_segment {
        TRACE("else_clause_list:ADD");
        add_ast_node_list_item($1, "list", $2);
    }
    ;

else_clause
    : else_clause_list {
        TRACE("else_clause: bare");
        $$ = create_ast_node(AST_ELSE_CLAUSE);
        add_ast_node_attrib($$, "else_clause_list", $1);
    }
    | else_clause_list final_else_segment {
        TRACE("else_clause: with final");
        $$ = create_ast_node(AST_ELSE_CLAUSE);
        add_ast_node_attrib($$, "else_clause_list", $1);
        add_ast_node_attrib($$, "final_else_segment", $2);

    }
    | final_else_segment {
        TRACE("else_clause:final_else_segment");
        $$ = create_ast_node(AST_ELSE_CLAUSE);
        add_ast_node_attrib($$, "final_else_segment", $1);
    }
    ;

while_clause
    : WHILE expression_param {
        TRACE("while_clause: with expr");
        $$ = create_ast_node(AST_WHILE_CLAUSE);
        add_ast_node_attrib($$, "expression_param", $2);
    }
    | WHILE {
        TRACE("while_clause: bare");
        $$ = create_ast_node(AST_WHILE_CLAUSE);
        //add_ast_node_attrib($$, "expression_param", NULL);
    }
    ;

while_statement
    : while_clause loop_body {
        TRACE("while_statement");
        $$ = create_ast_node(AST_WHILE_STATEMENT);
        add_ast_node_attrib($$, "while_clause", $1);
        add_ast_node_attrib($$, "loop_body", $2);
    }
    ;

do_statement
    : DO loop_body while_clause {
        TRACE("do_statement");
        $$ = create_ast_node(AST_DO_STATEMENT);
        add_ast_node_attrib($$, "while_clause", $3);
        add_ast_node_attrib($$, "loop_body", $2);
    }
    ;

assignment_right
    : expression {
        TRACE("assignment_right");
        int type = 0;
        $$ = create_ast_node(AST_ASSIGNMENT_RIGHT);
        add_ast_node_attrib($$, "expression", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | '(' type_name ')' expression {
        TRACE("assignment_right: as a cast");
        int type = 1;
        $$ = create_ast_node(AST_ASSIGNMENT_RIGHT);
        add_ast_node_attrib($$, "expression", $4);
        add_ast_node_attrib($$, "type_name", $2);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | list_init {
        TRACE("assignment_right:list_init");
        int type = 2;
        $$ = create_ast_node(AST_ASSIGNMENT_RIGHT);
        add_ast_node_attrib($$, "list_init", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | dict_init {
        TRACE("assignment_right:dict_init");
        int type = 3;
        $$ = create_ast_node(AST_ASSIGNMENT_RIGHT);
        add_ast_node_attrib($$, "dict_init", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    ;

list_init
    : '[' expression_list ']' {
        TRACE("list_init");
        $$ = create_ast_node(AST_LIST_INIT);
        add_ast_node_attrib($$, "expression_list", $2);
    }
    ;

dict_init_item
    : STRING_LIT ':' expression {
        TRACE("dict_init_item");
        $$ = create_ast_node(AST_DICT_INIT_ITEM);
        add_ast_node_attrib($$, "token", $1);
        add_ast_node_attrib($$, "expression", $3);
    }
    ;

dict_init_item_list
    : dict_init_item {
        TRACE("dict_init_item_list: CREATE");
        $$ = create_ast_node(AST_DICT_INIT_ITEM_LIST);
        add_ast_node_list_item($$, "list", $1);
    }
    | dict_init_item_list ',' dict_init_item {
        TRACE("dict_init_item_list: ADD");
        add_ast_node_list_item($1, "list", $3);
    }
    ;

dict_init
    : '[' dict_init_item_list ']' {
        TRACE("dict_init");
        $$ = create_ast_node(AST_DICT_INIT);
        add_ast_node_attrib($$, "dict_init_item_list", $2);
    }
    ;

    /*
     * Identifier is a reference that has to be examined when the variable
     * references are verified.
     */
assignment_left
    : IDENTIFIER {
        TRACE("assignment_left: identifier %s", $1);
        int type = 0;
        $$ = create_ast_node(AST_ASSIGNMENT_LEFT);
        add_ast_node_attrib($$, "token", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    | list_reference {
        TRACE("assignment_left: list");
        int type = 1;
        $$ = create_ast_node(AST_ASSIGNMENT_LEFT);
        add_ast_node_attrib($$, "list_reference", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, int));
    }
    ;

assignment
    : assignment_left '=' assignment_right {
        TRACE("assignment");
        $$ = create_ast_node(AST_ASSIGNMENT);
        add_ast_node_attrib($$, "assignment_right", $3);
        add_ast_node_attrib($$, "assignment_left", $1);
    }
    ;

    /*
     * references only.
     */
raw_value
    : IDENTIFIER {
        TRACE("raw_value:IDENTIFIER %s", $1);
        $$ = create_ast_node(AST_RAW_VALUE);
        add_ast_node_attrib($$, "token", $1);
    }
    | INTEGER_LIT {
        TRACE("raw_value:INTEGER_LIT %ld", $1);
        $$ = create_ast_node(AST_RAW_VALUE);
        add_ast_node_attrib($$, "token", $1);
    }
    | FLOAT_LIT {
        TRACE("raw_value:FLOAT_LIT %lf", $1);
        $$ = create_ast_node(AST_RAW_VALUE);
        add_ast_node_attrib($$, "token", $1);
    }
    ;

expr_primary
    : raw_value {
        TRACE("expr_primary:raw_value");
        ast_type_t type = AST_RAW_VALUE;
        $$ = create_ast_node(AST_EXPR_PRIMARY);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    | formatted_string {
        TRACE("expr_primary:formatted_string");
        ast_type_t type = AST_FORMATTED_STRING;
        $$ = create_ast_node(AST_EXPR_PRIMARY);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    | list_reference {
        TRACE("expr_primary:list_reference");
        ast_type_t type = AST_LIST_REFERENCE;
        $$ = create_ast_node(AST_EXPR_PRIMARY);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    | func_reference {
        TRACE("expr_primary:func_reference");
        ast_type_t type = AST_FUNC_REFERENCE;
        $$ = create_ast_node(AST_EXPR_PRIMARY);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    | '(' expression ')' {
        TRACE("expr_primary:(expression)");
        ast_type_t type = AST_EXPRESSION;
        $$ = create_ast_node(AST_EXPR_PRIMARY);
        add_ast_node_attrib($$, "node", $2);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    ;

    /*
     * This includes hashes as well as lists, since they are the same syntax.
     */
list_reference
    : IDENTIFIER list_ref_param_list {
        TRACE("list_reference: %s", $1);
        $$ = create_ast_node(AST_LIST_REFERENCE);
        add_ast_node_attrib($$, "list_ref_param_list", $2);
        add_ast_node_attrib($$, "token", $1);
    }
    ;

    /*
     * These have to be throurally check for symantic correctness.
     */
list_ref_value
    : IDENTIFIER {
        TRACE("list_ref_value:IDENTIFIER");
        int type = 0;
        $$ = create_ast_node(AST_LIST_REF_VALUE);
        add_ast_node_attrib($$, "token", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    | INTEGER_LIT {
        TRACE("list_ref_value:INTEGER_LIT");
        int type = 0;
        $$ = create_ast_node(AST_LIST_REF_VALUE);
        add_ast_node_attrib($$, "token", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    | STRING_LIT {
        TRACE("list_ref_value:STRING_LIT");
        int type = 0;
        $$ = create_ast_node(AST_LIST_REF_VALUE);
        add_ast_node_attrib($$, "token", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    | list_reference {
        TRACE("list_ref_value:list_reference");
        int type = 1;
        $$ = create_ast_node(AST_LIST_REF_VALUE);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    | func_reference {
        TRACE("list_ref_value:func_reference");
        int type = 2;
        $$ = create_ast_node(AST_LIST_REF_VALUE);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    | list_ref_param {
        TRACE("list_ref_value:list_ref_param");
        int type = 3;
        $$ = create_ast_node(AST_LIST_REF_VALUE);
        add_ast_node_attrib($$, "node", $1);
        add_ast_node_attrib($$, "type", _COPY_DS(&type, ast_type_t));
    }
    ;

list_ref_param
    : '[' list_ref_value ']' {
        TRACE("list_ref_param");
        $$ = create_ast_node(AST_LIST_REF_PARAM);
        add_ast_node_attrib($$, "list_ref_value", $2);
    }
    ;

list_ref_param_list
    : list_ref_param {
        TRACE("list_ref_param_list: CREATE");
        $$ = create_ast_node(AST_LIST_REF_PARAM_LIST);
        add_ast_node_list_item($$, "list", $1);
    }
    | list_ref_param_list list_ref_param {
        TRACE("list_ref_param_list: ADD");
        add_ast_node_list_item($1, "list", $2);
    }
    ;

func_reference
    : IDENTIFIER expression_list_param {
        TRACE("func_reference %s", $1);
        $$ = create_ast_node(AST_FUNC_REFERENCE);
        add_ast_node_attrib($$, "expression_list_param", $2);
        add_ast_node_attrib($$, "token", $1);
    }
    ;

expression
    : expr_primary {
        TRACE("expression:expr_primary");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "expr_primary", $1);
    }
    | expression ADD_OPER expression {
        TRACE("expression:+");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression SUB_OPER expression {
        TRACE("expression:-");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression MUL_OPER expression {
        TRACE("expression:*");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression DIV_OPER expression {
        TRACE("expression:/");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression MOD_OPER expression {
        TRACE("expression:%%");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression POW_OPER expression {
        TRACE("expression:^");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression GT_OPER expression {
        TRACE("expression:GT_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression LT_OPER expression {
        TRACE("expression:LT_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression EQU_OPER expression {
        TRACE("expression:EQU_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression NEQ_OPER expression {
        TRACE("expression:NEQ_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression LTE_OPER expression {
        TRACE("expression:LTE_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression GTE_OPER expression {
        TRACE("expression:GTE_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression OR_OPER expression {
        TRACE("expression:OR_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | expression AND_OPER expression {
        TRACE("expression:AND_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "left", $1);
        add_ast_node_attrib($$, "right", $3);
        add_ast_node_attrib($$, "oper", $2);
    }
    | NOT_OPER expression %prec UNARY {
        TRACE("expression:unary NOT_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "right", $2);
        add_ast_node_attrib($$, "oper", $1);
    }
    | SUB_OPER expression %prec UNARY {
        TRACE("expression:unary SUB_OPER");
        $$ = create_ast_node(AST_EXPRESSION);
        add_ast_node_attrib($$, "right", $2);
        add_ast_node_attrib($$, "oper", $1);
    }
    ;

expression_list
    : expression {
        TRACE("expression_list: CREATE");
        $$ = create_ast_node(AST_EXPRESSION_LIST);
        add_ast_node_list_item($$, "list", $1);
    }
    | expression_list ',' expression {
        TRACE("expression_list: ADD");
        add_ast_node_list_item($1, "list", $3);
    }
    ;

expression_list_param
    : '(' expression_list ')' {
        TRACE("expression_list_param: with params");
        $$ = create_ast_node(AST_EXPRESSION_LIST_PARAM);
        add_ast_node_attrib($$, "expression_list", $2);
    }
    | '(' ')' {
        TRACE("expression_list_param: bare");
        $$ = create_ast_node(AST_EXPRESSION_LIST_PARAM);
        //add_ast_node_attrib($$, "expression_list", NULL);
    }
    ;

expression_param
    : '(' expression ')' {
        TRACE("expression_param: with expression");
        $$ = create_ast_node(AST_EXPRESSION_PARAM);
        add_ast_node_attrib($$, "expression", $2);
    }
    | '(' ')' {
        TRACE("expression_param: bare");
        $$ = create_ast_node(AST_EXPRESSION_PARAM);
        //add_ast_node_attrib($$, "expression", NULL);
    }
    ;

%%

void yyerror(const char* s) {

    parser_syntax_error("%s", s);
}

const char* token_to_str(int tok) {

    return yysymbol_name(YYTRANSLATE(tok));
}
