#ifndef _AST_H_
#define _AST_H_

#include <stdbool.h>

#include "pointer_list.h"
#include "tokens.h"

typedef enum {
    AST_PROGRAM,
    AST_PROGRAM_ITEM_LIST,
    AST_PROGRAM_ITEM,
    AST_TYPE_NAME,
    AST_FORMATTED_STRING,
    AST_IMPORT_STATEMENT,
    AST_DATA_DECLARATION,
    AST_DATA_DECLARATION_LIST,
    AST_DATA_DEFINITION,
    AST_FUNC_DEFINITION,
    AST_FUNC_PARAMS,
    AST_FUNC_BODY,
    AST_FUNC_NAME,
    AST_FUNC_BODY_LIST,
    AST_LOOP_BODY,
    AST_LOOP_BODY_LIST,
    AST_LOOP_BODY_DIFFS,
    AST_LOOP_BODY_ELEM,
    AST_FUNC_BODY_ELEM,
    AST_TRACE_STATEMENT,
    AST_PRINT_STATEMENT,
    AST_EXIT_STATEMENT,
    AST_RETURN_STATEMENT,
    AST_TRYEXCEPT_STATEMENT,
    AST_TRY_CLAUSE,
    AST_EXCEPT_SEGMENT,
    AST_EXCEPT_CLAUSE_LIST,
    AST_FINAL_EXCEPT_CLAUSE,
    AST_EXCEPT_CLAUSE,
    AST_RAISE_STATEMENT,
    AST_FOR_STATEMENT,
    AST_IF_CLAUSE,
    AST_IFELSE_STATEMENT,
    AST_ELSE_SEGMENT,
    AST_FINAL_ELSE_SEGMENT,
    AST_ELSE_CLAUSE_LIST,
    AST_ELSE_CLAUSE,
    AST_WHILE_CLAUSE,
    AST_WHILE_STATEMENT,
    AST_DO_STATEMENT,
    AST_ASSIGNMENT_RIGHT,
    AST_ASSIGNMENT_LEFT,
    AST_ASSIGNMENT,
    AST_RAW_VALUE,
    AST_EXPR_PRIMARY,
    AST_LIST_REFERENCE,
    AST_LIST_REF_VALUE,
    AST_LIST_REF_PARAM,
    AST_LIST_REF_PARAM_LIST,
    AST_FUNC_REFERENCE,
    AST_EXPRESSION,
    AST_EXPRESSION_LIST,
    AST_EXPRESSION_LIST_PARAM,
    AST_EXPRESSION_PARAM,
    AST_DICT_INIT,
    AST_LIST_INIT,
    AST_DICT_INIT_ITEM,
    AST_DICT_INIT_ITEM_LIST,
    AST_EXCEPTION_IDENTIFIER,
} ast_type_t;

typedef struct {
    ast_type_t type;
} ast_node_t;

/*
 * program
 *    : program_item_list
 *    ;
 */
typedef struct _ast_program_t {
    ast_node_t node;
    struct _ast_program_item_list_t* program_item_list;
} ast_program_t;

/*
 * program_item_list
 *    : program_item
 *    | program_item_list program_item
 *    ;
 */
typedef struct _ast_program_item_list_t {
    ast_node_t node;
    pointer_list_t* list;
} ast_program_item_list_t;

/*
 * import_statement
 *    : IMPORT STRING_LIT
 *    ;
 */
typedef struct _ast_import_statement_t {
    ast_node_t node;
    token_t* STRING_LIT;
} ast_import_statement_t;

/*
 * program_item
 *    : data_definition
 *    | func_definition
 *    | func_body
 *    | import_statement
 *    | exception_identifier
 *    ;
 */
typedef struct _ast_program_item_t {
    ast_node_t node;
    ast_node_t* nterm;
} ast_program_item_t;

/*
 * exception_identifier
 *    : EXCEPT IDENTIFIER
 *    ;
 */
typedef struct _ast_exception_identifier_t {
    ast_node_t node;
    token_t* IDENTIFIER;
} ast_exception_identifier_t;

/*
 * type_name
 *    : INTEGER
 *    | FLOAT
 *    | STRING
 *    | LIST
 *    | HASH
 *    ;
 */
typedef struct _ast_type_name_t {
    ast_node_t node;
    token_t* token;
} ast_type_name_t;

/*
 * formatted_string
 *    : STRING_LIT expression_list_param
 *    | STRING_LIT
 *    ;
 */
typedef struct _ast_formatted_string_t {
    ast_node_t node;
    token_t* STRING_LIT;
    struct _ast_expression_list_param_t* expression_list_param;
} ast_formatted_string_t;

/*
 * data_declaration
 *    : type_name IDENTIFIER
 *    | CONST type_name IDENTIFIER
 *    ;
 */
typedef struct _ast_data_declaration_t {
    ast_node_t node;
    struct _ast_type_name_t* type_name;
    token_t* IDENTIFIER;
    bool is_const;
} ast_data_declaration_t;

/*
 * data_declaration_list
 *    : data_declaration
 *    | data_declaration_list ',' data_declaration
 *    ;
 */
typedef struct _ast_data_declaration_list_t {
    ast_node_t node;
    pointer_list_t* list;
} ast_data_declaration_list_t;

/*
 * data_definition
 *    : data_declaration
 *    | data_declaration '=' expression
 *    | data_declaration '=' list_init
 *    | data_declaration '=' dict_init
 *    ;
 */
typedef struct _ast_data_definition_t {
    ast_node_t node;
    struct _ast_data_declaration_t* data_declaration;
    ast_node_t* nterm;
    bool is_init;
} ast_data_definition_t;

/*
 * func_name
 *    : type_name IDENTIFIER
 *    | ITERATOR type_name IDENTIFIER
 *    | NOTHING IDENTIFIER
 *    ;
 */
typedef struct _ast_func_name_t {
    ast_node_t node;
    struct _ast_type_name_t* type_name;
    token_t* IDENTIFIER;
    bool is_iterator;
} ast_func_name_t;

/*
 * func_definition
 *    : func_name func_params func_body
 *    ;
 */
typedef struct _ast_func_definition_t {
    ast_node_t node;
    struct _ast_func_name_t* func_name;
    struct _ast_func_params_t* func_params;
    struct _ast_func_body_t* func_body;
} ast_func_definition_t;

/*
 * func_params
 *    : '(' data_declaration_list ')'
 *    | '(' ')'
 *    ;
 */
typedef struct _ast_func_params_t {
    ast_node_t node;
    struct _ast_data_declaration_list_t* data_declaration_list;
} ast_func_params_t;

/*
 * func_body
 *    : '{' func_body_list '}'
 *    ;
 */
typedef struct _ast_func_body_t {
    ast_node_t node;
    struct _ast_func_body_list_t* func_body_list;
} ast_func_body_t;

/*
 * func_body_list
 *    : func_body_elem
 *    | func_body_list func_body_elem
 *    ;
 */
typedef struct _ast_func_body_list_t {
    ast_node_t node;
    pointer_list_t* list;
} ast_func_body_list_t;

/*
 * loop_body
 *    : '{' '}'
 *    | '{' loop_body_list '}'
 *    ;
 */
typedef struct _ast_loop_body_t {
    ast_node_t node;
    struct _ast_loop_body_list_t* loop_body_list;
} ast_loop_body_t;

/*
 * loop_body_list
 *    : loop_body_elem
 *    | loop_body_list loop_body_elem
 *    ;
 */
typedef struct _ast_loop_body_list_t {
    ast_node_t node;
    pointer_list_t* list;
} ast_loop_body_list_t;

/*
 * loop_body_diffs
 *    : BREAK
 *    | CONTINUE
 *    | YIELD '(' expression ')'
 *    ;
 */
typedef struct _ast_loop_body_diffs_t {
    ast_node_t node;
    int type;
    struct _ast_expression_t* expression;
} ast_loop_body_diffs_t;

/*
 * loop_body_elem
 *    : func_body_elem
 *    | loop_body_diffs
 *    ;
 */
typedef struct _ast_loop_body_elem_t {
    ast_node_t node;
    ast_node_t* nterm;
} ast_loop_body_elem_t;

/*
 * func_body_elem
 *    : data_definition
 *    | func_reference
 *    | assignment
 *    | while_statement
 *    | do_statement
 *    | for_statement
 *    | ifelse_statement
 *    | tryexcept_statement
 *    | raise_statement
 *    | return_statement
 *    | exit_statement
 *    | print_statement
 *    | trace_statement
 *    | func_body
 *    ;
 */
typedef struct _ast_func_body_elem_t {
    ast_node_t node;
    ast_node_t* nterm;
} ast_func_body_elem_t;

/*
 * trace_statement
 *    : TRACE expression_list_param
 *    | TRACE
 *    ;
 */
typedef struct _ast_trace_statement_t {
    ast_node_t node;
    struct _ast_expression_list_parm_t* expression_list_param;
} ast_trace_statement_t;

/*
 * print_statement
 *    : PRINT expression_list_param
 *    | PRINT
 *    ;
 */
typedef struct _ast_print_statement_t {
    ast_node_t node;
    struct _ast_expression_list_parm_t* expression_list_param;
} ast_print_statement_t;

/*
 * exit_statement
 *    : EXIT expression_param
 *    ;
 */
typedef struct _ast_exit_statement_t {
    ast_node_t node;
    struct _ast_expression_parm_t* expression_param;
} ast_exit_statement_t;

/*
 * return_statement
 *    : RETURN expression_param
 *    | RETURN
 *    ;
 */
typedef struct _ast_return_statement_t {
    ast_node_t node;
    struct _ast_expression_parm_t* expression_param;
} ast_return_statement_t;

/*
 * for_statement
 *    : FOR '(' IDENTIFIER IN expression ')' func_body
 *    | FOR '(' type_name IDENTIFIER IN expression ')' func_body
 *    ;
 */
typedef struct _ast_for_statement_t {
    ast_node_t node;
    struct _ast_type_name_t* type_name;
    token_t* IDENTIFIER;
    struct _ast_expression_t* expression;
    struct _ast_func_body_t* func_body;
} ast_for_statement_t;

/*
 * tryexcept_statement
 *    : try_clause except_clause
 *    ;
 */
typedef struct _ast_tryexcept_statement_t {
    ast_node_t node;
    struct _ast_try_clause_t* try_clause;
    struct _ast_except_clause_t* except_clause;
} ast_tryexcept_statement_t;

/*
 * try_clause
 *    : TRY func_body
 *    ;
 */
typedef struct _ast_try_clause_t {
    ast_node_t node;
    struct _ast_func_body_t* func_body;
} ast_try_clause_t;

/*
 * except_segment
 *    : EXCEPT '(' IDENTIFIER ',' IDENTIFIER ')' func_body
 *    ;
 */
typedef struct _ast_except_segment_t {
    ast_node_t node;
    token_t* eident;
    token_t* mident;
    struct _ast_func_body_t* func_body;
} ast_except_segment_t;

/*
 * except_clause_list
 *    : except_segment
 *    | except_clause_list except_segment
 *    ;
 */
typedef struct _ast_except_clause_list_t {
    ast_node_t node;
    pointer_list_t* list;
} ast_except_clause_list_t;

/*
 * final_except_clause
 *    : EXCEPT '(' IDENTIFIER ')' func_body
 *    ;
 */
typedef struct _ast_final_except_clause_t {
    ast_node_t node;
    token_t* IDENTIFIER;
    struct _ast_func_body_t* func_body;
} ast_final_except_clause_t;

/*
 * except_clause
 *    : except_clause_list
 *    | except_clause_list final_except_clause
 *    | final_except_clause
 *    ;
 */
typedef struct _ast_except_clause_t {
    ast_node_t node;
    struct _ast_except_clause_list_t* except_clause_list;
    struct _ast_final_except_clause_t* final_except_clause;
} ast_except_clause_t;

/*
 * raise_statement
 *    : RAISE '(' IDENTIFIER ',' formatted_string ')'
 *    ;
 */
typedef struct _ast_raise_statement_t {
    ast_node_t node;
    token_t* IDENTIFIER;
    struct _ast_formatted_string_t* formatted_string;
} ast_raise_statement_t;

/*
 * if_clause
 *    : IF '(' expression ')' func_body
 *    ;
 */
typedef struct _ast_if_clause_t {
    ast_node_t node;
    struct _ast_expression_t* expression;
    struct _ast_func_body_t* func_body;
} ast_if_clause_t;

/*
 * ifelse_statement
 *    : if_clause
 *    | if_clause else_clause
 *    ;
 */
typedef struct _ast_ifelse_statement_t {
    ast_node_t node;
    struct _ast_if_clause_t* if_clause;
    struct _ast_else_clause_t* else_clause;
} ast_ifelse_statement_t;

/*
 * else_segment
 *    : ELSE '(' expression ')' func_body
 *    ;
 */
typedef struct _ast_else_segment_t {
    ast_node_t node;
    struct _ast_func_body_t* func_body;
    struct _ast_expression_t* expression;
} ast_else_segment_t;

/*
 * final_else_segment
 *    : ELSE '(' ')' func_body
 *    | ELSE func_body
 *    ;
 */
typedef struct _ast_final_else_segment_t {
    ast_node_t node;
    struct _ast_func_body_t* func_body;
} ast_final_else_segment_t;

/*
 * else_clause_list
 *    : else_segment
 *    | else_clause_list else_segment
 *    ;
 */
typedef struct _ast_else_clause_list_t {
    ast_node_t node;
    pointer_list_t* list;
} ast_else_clause_list_t;

/*
 * else_clause
 *    : else_clause_list
 *    | else_clause_list final_else_segment
 *    | final_else_segment
 *    ;
 */
typedef struct _ast_else_clause_t {
    ast_node_t node;
    struct _ast_else_clause_list_t* else_clause_list;
    struct _ast_final_else_segment_t* final_else_segment;
} ast_else_clause_t;

/*
 * while_clause
 *    : WHILE expression_param
 *    | WHILE
 *    ;
 */
typedef struct _ast_while_clause_t {
    ast_node_t node;
    struct _ast_expression_param_t* expression_param;
} ast_while_clause_t;

/*
 * while_statement
 *    : while_clause loop_body
 *    ;
 */
typedef struct _ast_while_statement_t {
    ast_node_t node;
    struct _ast_loop_body_t* loop_body;
    struct _ast_while_clause_t* while_clause;
} ast_while_statement_t;

/*
 * do_statement
 *    : DO loop_body while_clause
 *    ;
 */
typedef struct _ast_do_statement_t {
    ast_node_t node;
    struct _ast_loop_body_t* loop_body;
    struct _ast_while_clause_t* while_clause;
} ast_do_statement_t;

/*
 * assignment_right
 *    : expression
 *    | '(' type_name ')' expression
 *    | list_init
 *    | dict_init
 *    ;
 */
typedef struct _ast_assignment_right_t {
    ast_node_t node;
    ast_node_t* nterm;
    struct _ast_type_name_t* type_name;
} ast_assignment_right_t;

/*
 * list_init
 *    : '[' expression_list ']'
 *    ;
 */
typedef struct _ast_list_init_t {
    ast_node_t node;
    struct _ast_expression_list_t* expression_list;
} ast_list_init_t;

/*
 * dict_init_item
 *    : STRING_LIT ':' expression
 *    ;
 */
typedef struct _ast_dict_init_item_t {
    ast_node_t node;
    token_t* STRING_LIT;
    struct _ast_expression_t* expression;
} ast_dict_init_item_t;

/*
 * dict_init_item_list
 *    : dict_init_item
 *    | dict_init_item_list ',' dict_init_item
 *    ;
 */
typedef struct _ast_dict_init_item_list_t {
    ast_node_t node;
    pointer_list_t* list;
} ast_dict_init_item_list_t;

/*
 * dict_init
 *    : '[' dict_init_item_list ']'
 *    ;
 */
typedef struct _ast_dict_init_t {
    ast_node_t node;
    struct _ast_dict_init_item_list_t* dict_init_item_list;
} ast_dict_init_t;

/*
 * assignment_left
 *    : IDENTIFIER
 *    | list_reference
 *    ;
 */
typedef struct _ast_assignment_left_t {
    ast_node_t node;
    token_t* IDENTIFIER;
    struct _ast_list_reference_t* list_reference;
} ast_assignment_left_t;

/*
 * assignment
 *    : assignment_left '=' assignment_right
 *    ;
 */
typedef struct _ast_assignment_t {
    ast_node_t node;
    struct _ast_assignment_left_t* assignment_left;
    struct _ast_assignment_right_t* assignment_right;
} ast_assignment_t;

/*
 * raw_value
 *    : IDENTIFIER
 *    | INTEGER_LIT
 *    | FLOAT_LIT
 *    ;
 */
typedef struct _ast_raw_value_t {
    ast_node_t node;
    token_t* token;
} ast_raw_value_t;

/*
 * expr_primary
 *    : raw_value
 *    | formatted_string
 *    | list_reference
 *    | func_reference
 *    ;
 */
typedef struct _ast_expr_primary_t {
    ast_node_t node;
    ast_node_t* nterm;
} ast_expr_primary_t;

/*
 * list_reference
 *    : IDENTIFIER list_ref_param_list
 *    ;
 */
typedef struct _ast_list_reference_t {
    ast_node_t node;
    token_t* IDENTIFIER;
    struct _ast_list_ref_param_list_t* list_ref_param_list;
} ast_list_reference_t;

/*
 * list_ref_value
 *    : IDENTIFIER
 *    | INTEGER_LIT
 *    | STRING_LIT
 *    | list_reference
 *    | func_reference
 *    | list_ref_param
 *    ;
 */
typedef struct _ast_list_ref_value_t {
    ast_node_t node;
    token_t* token;
    ast_node_t* nterm;
} ast_list_ref_value_t;

/*
 * list_ref_param
 *    : '[' list_ref_value ']'
 *    ;
 */
typedef struct _ast_list_ref_param_t {
    ast_node_t node;
    struct _ast_list_ref_value_t* list_ref_value;
} ast_list_ref_param_t;

/*
 * list_ref_param_list
 *    : list_ref_param
 *    | list_ref_param_list list_ref_param
 *    ;
 */
typedef struct _ast_list_ref_param_list_t {
    ast_node_t node;
    pointer_list_t* list;
} ast_list_ref_param_list_t;

/*
 * func_reference
 *    : IDENTIFIER expression_list_param
 *    ;
 */
typedef struct _ast_func_reference_t {
    ast_node_t node;
    token_t* IDENTIFIER;
    struct _ast_expression_list_param_t* expression_list_param;
} ast_func_reference_t;

/*
 * expression
 *    : expr_primary
 *    | expression ADD_OPER expression
 *    | expression SUB_OPER expression
 *    | expression MUL_OPER expression
 *    | expression DIV_OPER expression
 *    | expression MOD_OPER expression
 *    | expression POW_OPER expression
 *    | expression GT_OPER expression
 *    | expression LT_OPER expression
 *    | expression EQU_OPER expression
 *    | expression NEQ_OPER expression
 *    | expression LTE_OPER expression
 *    | expression GTE_OPER expression
 *    | expression OR_OPER expression
 *    | expression AND_OPER expression
 *    | NOT_OPER expression %prec UNARY
 *    | SUB_OPER expression %prec UNARY
 *    | '(' expression ')'
 *    ;
 */
typedef struct _ast_expression_t {
    ast_node_t node;
    struct _ast_expression_t* left;
    struct _ast_expression_t* right;
    token_t* oper;
    struct _ast_expr_primary_t* primary;
    struct _ast_expression_t* expr;
} ast_expression_t;

/*
 * expression_list
 *    : expression
 *    | expression_list ',' expression
 *    ;
 */
typedef struct _ast_expression_list_t {
    ast_node_t node;
    pointer_list_t* list;
} ast_expression_list_t;

/*
 * expression_list_param
 *    : '(' expression_list ')'
 *    | '(' ')'
 *    ;
 */
typedef struct _ast_expression_list_param_t {
    ast_node_t node;
    struct _ast_expression_list_t* expression_list;
} ast_expression_list_param_t;

/*
 * expression_param
 *    : '(' expression ')'
 *    | '(' ')'
 *    ;
 */
typedef struct _ast_expression_param_t {
    ast_node_t node;
    struct _ast_expression_t* expression;
} ast_expression_param_t;



ast_node_t* create_ast_node(ast_type_t type);
void add_ast_node_attrib(ast_node_t* node, const char* key, void* attrib);
int get_ast_node_attrib(ast_node_t* node, const char* key, ast_node_t** val);
void add_ast_node_list_item(ast_node_t* node, const char* key, void* attrib);
void traverse_ast(void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
const char* node_type_to_str(ast_node_t* node);
const char* node_type_to_name(ast_node_t* node);

ast_node_t* alloc_ast_node(ast_type_t type);

#endif /* _AST_H_ */
