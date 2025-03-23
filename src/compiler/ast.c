/**
 * @file ast.c
 *
 * @brief Implement the Abstract Syntax Tree
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-02-28
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include "common_errors.h"
#include "common_pointer_list.h"
#include "ast.h"
#include "parser.h"
#include "common_memory.h"

// #define AST_TRACE

#ifdef AST_TRACE
#define USE_TRACE
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#endif
#include "trace.h"

// global used as the return value of the parser.
ast_program_t* root_node = NULL;

#define AST_ENTER(t)                                                                                                    \
    do {                                                                                                                \
        if(node == NULL)                                                                                                \
            return;                                                                                                     \
        ENTER;                                                                                                          \
        ASSERT(node->node.type == t, "expected node type %s but got type %s", #t, node_type_to_str((ast_node_t*)node)); \
        if(pre != NULL)                                                                                                 \
            (*pre)((ast_node_t*)node);                                                                                  \
    } while(0)

#define AST_RETURN                      \
    do {                                \
        if(post != NULL)                \
            (*post)((ast_node_t*)node); \
        RETURN();                       \
    } while(0)

#define TRAVERSE(name)                                             \
    do {                                                           \
        traverse_##name((ast_##name##_t*)(node->name), pre, post); \
    } while(0)

#define SWITCH              \
    if(node->nterm != NULL) \
        switch(node->nterm->type)

#define CASE(type, name)                                            \
    case type:                                                      \
        traverse_##name((ast_##name##_t*)(node->nterm), pre, post); \
        break

#define DEFAULT \
    default:    \
        FATAL("unknown state: %s (%d)", node_type_to_str(node->nterm), node->nterm->type)

#define TRAVERSE_LIST(name)                                             \
    do {                                                                \
        int mark = 0;                                                   \
        ast_##name##_t* item;                                           \
        while(NULL != (item = iterate_pointer_list(node->list, &mark))) \
            traverse_##name(item, pre, post);                           \
    } while(0)

#ifdef AST_TRACE

#define TRACE_TOKEN(t)                                                                                \
    do {                                                                                              \
        if(node->t != NULL)                                                                           \
            TRACE("token: %s:%d:%d: %s: %s", (node->t)->fname, (node->t)->line_no, (node->t)->col_no, \
                  token_to_str((node->t)->type), (node->t)->raw);                                     \
    } while(0)

#define TRACE_BOOL(v)                                             \
    do {                                                          \
        TRACE("bool: %s = %s", #v, (node->v) ? "TRUE" : "FALSE"); \
    } while(0)

#define TRACE_TERM(t)                                 \
    do {                                              \
        TRACE("terminal: %s", token_to_str(node->t)); \
    } while(0)

#else
#define TRACE_TOKEN(v)
#define TRACE_BOOL(v)
#define TRACE_TERM(v)
#endif

static inline void traverse_program(ast_program_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_program_item_list(ast_program_item_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_import_statement(ast_import_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_program_item(ast_program_item_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_start_block(ast_start_block_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_type_name(ast_type_name_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_formatted_string(ast_formatted_string_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_data_declaration(ast_data_declaration_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_data_declaration_list(ast_data_declaration_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_data_definition(ast_data_definition_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_func_name(ast_func_name_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_func_definition(ast_func_definition_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_func_params(ast_func_params_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_func_body(ast_func_body_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_func_body_list(ast_func_body_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_loop_body(ast_loop_body_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_loop_body_list(ast_loop_body_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_loop_body_diffs(ast_loop_body_diffs_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_loop_body_elem(ast_loop_body_elem_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_func_body_elem(ast_func_body_elem_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_inline_statement(ast_inline_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_return_statement(ast_return_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_if_clause(ast_if_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_ifelse_statement(ast_ifelse_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_else_segment(ast_else_segment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_final_else_segment(ast_final_else_segment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_else_clause_list(ast_else_clause_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_else_clause(ast_else_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_while_clause(ast_while_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_while_statement(ast_while_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_do_statement(ast_do_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_assignment_right(ast_assignment_right_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_assignment(ast_assignment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_raw_value(ast_raw_value_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_expr_primary(ast_expr_primary_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_func_reference(ast_func_reference_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_expression(ast_expression_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_expression_list(ast_expression_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_expression_list_param(ast_expression_list_param_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_expression_param(ast_expression_param_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));

/*
 * program
 *    : program_item_list
 *    ;
 */
static inline void traverse_program(ast_program_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_PROGRAM);

    TRAVERSE(program_item_list);

    AST_RETURN;
}

/*
 * program_item_list
 *    : program_item
 *    | program_item_list program_item
 *    ;
 */
static inline void traverse_program_item_list(ast_program_item_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_PROGRAM_ITEM_LIST);

    TRAVERSE_LIST(program_item);

    AST_RETURN;
}

/*
 * import_statement
 *    : IMPORT STRING_LIT
 *    ;
 */
static inline void traverse_import_statement(ast_import_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_IMPORT_STATEMENT);

    TRACE_TOKEN(STRING_LIT);

    AST_RETURN;
}

/*
 * program_item
 *    : data_definition
 *    | func_definition
 *    | func_body
 *    | import_statement
 *    | exception_identifier
 *    ;
 */
static inline void traverse_program_item(ast_program_item_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_PROGRAM_ITEM);

    SWITCH {
        CASE(AST_DATA_DECLARATION, data_declaration);
        CASE(AST_FUNC_DEFINITION, func_definition);
        CASE(AST_FUNC_BODY, func_body);
        CASE(AST_IMPORT_STATEMENT, import_statement);
        CASE(AST_START_BLOCK, start_block);
        DEFAULT;
    }

    AST_RETURN;
}

/*
 * start_block
 *    : START func_body
 *    ;
 */
static inline void traverse_start_block(ast_start_block_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_START_BLOCK);

    TRAVERSE(func_body);

    AST_RETURN;
}

/*
 * type_name
 *    : INTEGER
 *    | FLOAT
 *    | STRING
 *    | LIST
 *    | HASH
 *    ;
 */
static inline void traverse_type_name(ast_type_name_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_TYPE_NAME);

    TRACE_TOKEN(token);

    AST_RETURN;
}

/*
 * formatted_string
 *    : STRING_LIT expression_list_param
 *    | STRING_LIT
 *    ;
 */
static inline void traverse_formatted_string(ast_formatted_string_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FORMATTED_STRING);

    TRACE_TOKEN(STRING_LIT);
    TRAVERSE(expression_list_param);

    AST_RETURN;
}

/*
 * Generate a data definition symbol.
 *
 * data_declaration
 *    : type_name IDENTIFIER
 *    | CONST type_name IDENTIFIER
 *    ;
 */
static inline void traverse_data_declaration(ast_data_declaration_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_DATA_DECLARATION);

    TRACE_TOKEN(IDENTIFIER);
    TRACE_BOOL(is_const);
    TRAVERSE(type_name);

    AST_RETURN;
}

/*
 * data_declaration_list
 *    : data_declaration
 *    | data_declaration_list ',' data_declaration
 *    ;
 */
static inline void traverse_data_declaration_list(ast_data_declaration_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_DATA_DECLARATION_LIST);

    TRAVERSE_LIST(data_declaration);

    AST_RETURN;
}

/*
 * data_definition
 *    : data_declaration
 *    | data_declaration '=' expression
 *    | data_declaration '=' list_init
 *    | data_declaration '=' dict_init
 *    ;
 */
static inline void traverse_data_definition(ast_data_definition_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_DATA_DEFINITION);

    TRAVERSE(data_declaration);
    TRAVERSE(expression);
    TRACE_BOOL(is_init);

    AST_RETURN;
}

/*
 * Generate a function name symbol, but the decorated symbol name
 * cannot be generated until the parameters have been parsed.
 *
 * func_name
 *    : type_name IDENTIFIER
 *    | ITERATOR type_name IDENTIFIER
 *    | NOTHING IDENTIFIER
 *    ;
 */
static inline void traverse_func_name(ast_func_name_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FUNC_NAME);

    TRACE_TOKEN(IDENTIFIER);
    TRAVERSE(type_name);

    AST_RETURN;
}

/*
 * Add the parameters in their own symbol context and finish generating
 * the decorated function name.
 *
 * func_definition
 *    : func_name func_params func_body
 *    ;
 */
static inline void traverse_func_definition(ast_func_definition_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FUNC_DEFINITION);

    TRAVERSE(func_name);
    TRAVERSE(func_params);
    TRAVERSE(func_body);

    AST_RETURN;
}

/*
 * func_params
 *    : '(' data_declaration_list ')'
 *    | '(' ')'
 *    ;
 */
static inline void traverse_func_params(ast_func_params_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FUNC_PARAMS);

    TRAVERSE(data_declaration_list);

    AST_RETURN;
}

/*
 * func_body
 *    : '{' func_body_list '}'
 *    ;
 */
static inline void traverse_func_body(ast_func_body_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FUNC_BODY);

    TRAVERSE(func_body_list);

    AST_RETURN;
}

/*
 * func_body_list
 *    : func_body_elem
 *    | func_body_list func_body_elem
 *    ;
 */
static inline void traverse_func_body_list(ast_func_body_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FUNC_BODY_LIST);

    TRAVERSE_LIST(func_body_elem);

    AST_RETURN;
}

/*
 * loop_body
 *    : '{' '}'
 *    | '{' loop_body_list '}'
 *    ;
 */
static inline void traverse_loop_body(ast_loop_body_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_LOOP_BODY);

    TRAVERSE(loop_body_list);

    AST_RETURN;
}

/*
 * loop_body_list
 *    : loop_body_elem
 *    | loop_body_list loop_body_elem
 *    ;
 */
static inline void traverse_loop_body_list(ast_loop_body_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_LOOP_BODY_LIST);

    TRAVERSE_LIST(loop_body_elem);

    AST_RETURN;
}

/*
 * loop_body_diffs
 *    : BREAK
 *    | CONTINUE
 *    | YIELD '(' expression ')'
 *    ;
 */
static inline void traverse_loop_body_diffs(ast_loop_body_diffs_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_LOOP_BODY_DIFFS);

    TRACE_TERM(type);
    TRAVERSE(expression);

    AST_RETURN;
}

/*
 * loop_body_elem
 *    : func_body_elem
 *    | loop_body_diffs
 *    ;
 */
static inline void traverse_loop_body_elem(ast_loop_body_elem_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_LOOP_BODY_ELEM);

    SWITCH {
        CASE(AST_LOOP_BODY_DIFFS, loop_body_diffs);
        CASE(AST_FUNC_BODY_ELEM, func_body_elem);
        DEFAULT;
    }

    AST_RETURN;
}

/*
 * func_body_elem
 *    : data_definition
 *    | func_reference
 *    | assignment
 *    | while_statement
 *    | do_statement
 *    | for_statement
 *    | ifelse_statement
 *    | return_statement
 *    | exit_statement
 *    | print_statement
 *    | trace_statement
 *    | func_body
 *    ;
 */
static inline void traverse_func_body_elem(ast_func_body_elem_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FUNC_BODY_ELEM);

    SWITCH {
        CASE(AST_DATA_DEFINITION, data_definition);
        CASE(AST_FUNC_REFERENCE, func_reference);
        CASE(AST_ASSIGNMENT, assignment);
        CASE(AST_WHILE_STATEMENT, while_statement);
        CASE(AST_DO_STATEMENT, do_statement);
        CASE(AST_IFELSE_STATEMENT, ifelse_statement);
        CASE(AST_RETURN_STATEMENT, return_statement);
        CASE(AST_INLINE_STATEMENT, inline_statement);
        CASE(AST_FUNC_BODY, func_body);
        DEFAULT;
    }

    AST_RETURN;
}

/*
 * inline_statement
 *    : INLINE
 *    ;
 */
static inline void traverse_inline_statement(ast_inline_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_INLINE_STATEMENT);

    TRACE_TOKEN(token);

    AST_RETURN;
}

/*
 * return_statement
 *    : RETURN expression_param
 *    | RETURN
 *    ;
 */
static inline void traverse_return_statement(ast_return_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_RETURN_STATEMENT);

    TRAVERSE(expression_param);

    AST_RETURN;
}


/*
 * if_clause
 *    : IF '(' expression ')' func_body
 *    ;
 */
static inline void traverse_if_clause(ast_if_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_IF_CLAUSE);

    TRAVERSE(expression);
    TRAVERSE(func_body);

    AST_RETURN;
}

/*
 * ifelse_statement
 *    : if_clause
 *    | if_clause else_clause
 *    ;
 */
static inline void traverse_ifelse_statement(ast_ifelse_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_IFELSE_STATEMENT);

    TRAVERSE(if_clause);
    TRAVERSE(else_clause);

    AST_RETURN;
}

/*
 * else_segment
 *    : ELSE '(' expression ')' func_body
 *    ;
 */
static inline void traverse_else_segment(ast_else_segment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_ELSE_SEGMENT);

    TRAVERSE(expression);
    TRAVERSE(func_body);

    AST_RETURN;
}

/*
 * final_else_segment
 *    : ELSE '(' ')' func_body
 *    | ELSE func_body
 *    ;
 */
static inline void traverse_final_else_segment(ast_final_else_segment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FINAL_ELSE_SEGMENT);

    TRAVERSE(func_body);

    AST_RETURN;
}

/*
 * else_clause_list
 *    : else_segment
 *    | else_clause_list else_segment
 *    ;
 */
static inline void traverse_else_clause_list(ast_else_clause_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_ELSE_CLAUSE_LIST);

    TRAVERSE_LIST(else_segment);

    AST_RETURN;
}

/*
 * else_clause
 *    : else_clause_list
 *    | else_clause_list final_else_segment
 *    | final_else_segment
 *    ;
 */
static inline void traverse_else_clause(ast_else_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_ELSE_CLAUSE);

    TRAVERSE(else_clause_list);
    TRAVERSE(final_else_segment);

    AST_RETURN;
}

/*
 * while_clause
 *    : WHILE expression_param
 *    | WHILE
 *    ;
 */
static inline void traverse_while_clause(ast_while_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_WHILE_CLAUSE);

    TRAVERSE(expression_param);

    AST_RETURN;
}

/*
 * while_statement
 *    : while_clause loop_body
 *    ;
 */
static inline void traverse_while_statement(ast_while_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_WHILE_STATEMENT);

    TRAVERSE(while_clause);
    TRAVERSE(loop_body);

    AST_RETURN;
}

/*
 * do_statement
 *    : DO loop_body while_clause
 *    ;
 */
static inline void traverse_do_statement(ast_do_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_DO_STATEMENT);

    TRAVERSE(loop_body);
    TRAVERSE(while_clause);

    AST_RETURN;
}

/*
 * assignment_right
 *    : expression
 *    | '(' type_name ')' expression
 *    | list_init
 *    | dict_init
 *    ;
 */
static inline void traverse_assignment_right(ast_assignment_right_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_ASSIGNMENT_RIGHT);

    TRAVERSE(expression);
    TRAVERSE(type_name);

    AST_RETURN;
}


/*
 * assignment
 *    : assignment_left '=' assignment_right
 *    ;
 */
static inline void traverse_assignment(ast_assignment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_ASSIGNMENT);

    TRAVERSE(assignment_right);
    TRACE_TOKEN(IDENTIFIER);

    AST_RETURN;
}

/*
 * references only.
 *
 * raw_value
 *    : IDENTIFIER
 *    | INTEGER_LIT
 *    | FLOAT_LIT
 *    ;
 */
static inline void traverse_raw_value(ast_raw_value_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_RAW_VALUE);

    TRACE_TOKEN(token);

    AST_RETURN;
}

/*
 * expr_primary
 *    : raw_value
 *    | formatted_string
 *    ;
 */
static inline void traverse_expr_primary(ast_expr_primary_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXPR_PRIMARY);

    SWITCH {
        CASE(AST_RAW_VALUE, raw_value);
        CASE(AST_FORMATTED_STRING, formatted_string);
        CASE(AST_FUNC_REFERENCE, func_reference);
        DEFAULT;
    }

    AST_RETURN;
}

/*
 * func_reference
 *    : IDENTIFIER expression_list_param
 *    ;
 */
static inline void traverse_func_reference(ast_func_reference_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FUNC_REFERENCE);

    TRACE_TOKEN(IDENTIFIER);
    TRAVERSE(expression_list_param);

    AST_RETURN;
}

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
static inline void traverse_expression(ast_expression_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXPRESSION);

    if(node->expr_primary != NULL) {
        traverse_expr_primary(node->expr_primary, pre, post);
    }
    else if(node->expression != NULL) {
        traverse_expression(node->expression, pre, post);
    }
    else if(node->oper->type == NOT_OPER) {
        TRACE_TOKEN(oper);
        traverse_expression(node->right, pre, post);
    }
    else if(node->oper->type == UNARY_MINUS_OPER) {
        TRACE_TOKEN(oper);
        traverse_expression(node->right, pre, post);
    }
    else {
        TRACE_TOKEN(oper);
        traverse_expression(node->left, pre, post);
        traverse_expression(node->right, pre, post);
    }

    AST_RETURN;
}

/*
 * expression_list
 *    : expression
 *    | expression_list ',' expression
 *    ;
 */
static inline void traverse_expression_list(ast_expression_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXPRESSION_LIST);

    TRAVERSE_LIST(expression);

    AST_RETURN;
}

/*
 * expression_list_param
 *    : '(' expression_list ')'
 *    | '(' ')'
 *    ;
 */
static inline void
        traverse_expression_list_param(ast_expression_list_param_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXPRESSION_LIST_PARAM);

    TRAVERSE(expression_list);

    AST_RETURN;
}

/*
 * expression_param
 *    : '(' expression ')'
 *    | '(' ')'
 *    ;
 */
static inline void traverse_expression_param(ast_expression_param_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXPRESSION_PARAM);

    TRAVERSE(expression);

    AST_RETURN;
}

/******************************************************************************
 * PUBLIC INTERFACE
 *****************************************************************************/

int get_line_no(void);
int get_col_no(void);
const char* get_file_name(void);

extern int parser_line;
extern char* parser_file;

void traverse_ast(void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    SEPARATOR;
    ENTER;

    ASSERT(root_node != NULL, "root node is NULL!");
    traverse_program(root_node, pre, post);

    RETURN();
}

ast_node_t* create_ast_node(ast_type_t type) {

    ast_node_t* ptr = alloc_ast_node(type);

    ptr->type  = type;
    //ptr->line  = get_line_no();
    ptr->line  = parser_line;
    //ptr->col   = get_col_no();
    //ptr->fname = get_file_name();
    ptr->fname = _COPY_STRING(parser_file);

    return ptr;
}

ast_program_t* get_program(void) {

    return root_node;
}