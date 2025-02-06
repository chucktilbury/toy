#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include "errors.h"
#include "fileio.h"
#include "hash.h"
#include "memory.h"
#include "pointer_list.h"
#include "ast.h"
#include "parser.h"

#define USE_TRACE
#include "trace.h"

// global used as the return value of the parser.
ast_program_t* root_node = NULL;

#define AST_ENTER(t)                                                                                                    \
    do {                                                                                                                \
        if(node == NULL)                                                                                                \
            return;                                                                                                     \
        ASSERT(node->node.type == t, "expected node type %s but got type %s", #t, node_type_to_str((ast_node_t*)node)); \
        ENTER;                                                                                                          \
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

#define SWITCH switch(node->nterm->type)

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

#define TRACE_TOKEN(t)                                                                                  \
    do {                                                                                                \
        if(node->t != NULL)                                                                             \
            TRACE("token: %s:%d:%d: %s: %s\n", (node->t)->fname, (node->t)->line_no, (node->t)->col_no, \
                  token_to_str((node->t)->type), (node->t)->raw);                                       \
    } while(0)

#define TRACE_BOOL(v)                                             \
    do {                                                          \
        TRACE("bool: %s = %s", #v, (node->v) ? "TRUE" : "FALSE"); \
    } while(0)

#define TRACE_TERM(t)                                 \
    do {                                              \
        TRACE("terminal: %s", token_to_str(node->t)); \
    } while(0)


static inline void traverse_program(ast_program_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_program_item_list(ast_program_item_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_import_statement(ast_import_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_program_item(ast_program_item_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void
        traverse_exception_identifier(ast_exception_identifier_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_type_name(ast_type_name_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_formatted_string(ast_formatted_string_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_data_declaration(ast_data_declaration_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void
        traverse_data_declaration_list(ast_data_declaration_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
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
static inline void traverse_trace_statement(ast_trace_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_print_statement(ast_print_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_exit_statement(ast_exit_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_return_statement(ast_return_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_for_statement(ast_for_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_tryexcept_statement(ast_tryexcept_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_try_clause(ast_try_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_except_segment(ast_except_segment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_except_clause_list(ast_except_clause_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_final_except_clause(ast_final_except_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_except_clause(ast_except_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_raise_statement(ast_raise_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
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
static inline void traverse_list_init(ast_list_init_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_dict_init_item(ast_dict_init_item_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_dict_init_item_list(ast_dict_init_item_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_dict_init(ast_dict_init_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_assignment_left(ast_assignment_left_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_assignment(ast_assignment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_raw_value(ast_raw_value_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_expr_primary(ast_expr_primary_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_list_reference(ast_list_reference_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_list_ref_value(ast_list_ref_value_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_list_ref_param(ast_list_ref_param_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_list_ref_param_list(ast_list_ref_param_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_func_reference(ast_func_reference_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_expression(ast_expression_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void traverse_expression_list(ast_expression_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static inline void
        traverse_expression_list_param(ast_expression_list_param_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
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
        CASE(AST_DATA_DEFINITION, data_definition);
        CASE(AST_FUNC_DEFINITION, func_definition);
        CASE(AST_FUNC_BODY, func_body);
        CASE(AST_IMPORT_STATEMENT, import_statement);
        CASE(AST_EXCEPTION_IDENTIFIER, exception_identifier);
        DEFAULT;
    }

    AST_RETURN;
}

/*
 * Create a exception handler ID symbol.
 *
 * exception_identifier
 *    : EXCEPT IDENTIFIER
 *    ;
 */
static inline void
        traverse_exception_identifier(ast_exception_identifier_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXCEPTION_IDENTIFIER);

    TRACE_TOKEN(IDENTIFIER);

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
static inline void
        traverse_data_declaration_list(ast_data_declaration_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

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
    TRACE_BOOL(is_init);

    SWITCH {
        CASE(AST_EXPRESSION, expression);
        CASE(AST_LIST_INIT, list_init);
        CASE(AST_DICT_INIT, dict_init);
        DEFAULT;
    }

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
        CASE(AST_LOOP_BODY_DIFFS, func_body_elem);
        CASE(AST_FUNC_BODY_ELEM, loop_body_diffs);
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
 *    | tryexcept_statement
 *    | raise_statement
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
        CASE(AST_FOR_STATEMENT, for_statement);
        CASE(AST_IFELSE_STATEMENT, ifelse_statement);
        CASE(AST_TRYEXCEPT_STATEMENT, tryexcept_statement);
        CASE(AST_RAISE_STATEMENT, raise_statement);
        CASE(AST_RETURN_STATEMENT, return_statement);
        CASE(AST_EXIT_STATEMENT, exit_statement);
        CASE(AST_PRINT_STATEMENT, print_statement);
        CASE(AST_TRACE_STATEMENT, trace_statement);
        CASE(AST_FUNC_BODY, func_body);
        DEFAULT;
    }

    AST_RETURN;
}

/*
 * trace_statement
 *    : TRACE expression_list_param
 *    | TRACE
 *    ;
 */
static inline void traverse_trace_statement(ast_trace_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_TRACE_STATEMENT);

    TRAVERSE(expression_list_param);

    AST_RETURN;
}

/*
 * print_statement
 *    : PRINT expression_list_param
 *    | PRINT
 *    ;
 */
static inline void traverse_print_statement(ast_print_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_PRINT_STATEMENT);

    TRAVERSE(expression_list_param);

    AST_RETURN;
}

/*
 * exit_statement
 *    : EXIT expression_param
 *    ;
 */
static inline void traverse_exit_statement(ast_exit_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXIT_STATEMENT);

    TRAVERSE(expression_param);

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
 * If the type name is specified then this is a symbol definition, but
 * if not, then it is a symbol reference. Either way it gets it's own
 * symbol context similar to a function definition.
 *
 * for_statement
 *    : FOR '(' IDENTIFIER IN expression ')' func_body
 *    | FOR '(' type_name IDENTIFIER IN expression ')' func_body
 *    ;
 */
static inline void traverse_for_statement(ast_for_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FOR_STATEMENT);

    TRACE_TOKEN(IDENTIFIER);
    TRAVERSE(type_name);
    TRAVERSE(expression);
    TRAVERSE(func_body);

    AST_RETURN;
}

/*
 * tryexcept_statement
 *    : try_clause except_clause
 *    ;
 */
static inline void traverse_tryexcept_statement(ast_tryexcept_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_TRYEXCEPT_STATEMENT);

    TRAVERSE(try_clause);
    TRAVERSE(except_clause);

    AST_RETURN;
}

/*
 * try_clause
 *    : TRY func_body
 *    ;
 */
static inline void traverse_try_clause(ast_try_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_TRY_CLAUSE);

    TRAVERSE(func_body);

    AST_RETURN;
}

/*
 * The first identifier is a reference to an expression handler ID and the
 * second identifier is a symbol definition of a string that requires its
 * own symbol context, similar to a function definition.
 *
 * except_segment
 *    : EXCEPT '(' IDENTIFIER ',' IDENTIFIER ')' func_body
 *    ;
 */
static inline void traverse_except_segment(ast_except_segment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXCEPT_SEGMENT);

    TRACE_TOKEN(eident);
    TRACE_TOKEN(mident);
    TRAVERSE(func_body);

    AST_RETURN;
}

/*
 * except_clause_list
 *    : except_segment
 *    | except_clause_list except_segment
 *    ;
 */
static inline void traverse_except_clause_list(ast_except_clause_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXCEPT_CLAUSE_LIST);

    TRAVERSE_LIST(except_segment);

    AST_RETURN;
}

/*
 * The identifier is a definition of a string symbol, same as the second
 * identifier in a except_segment.
 *
 * final_except_clause
 *    : EXCEPT '(' IDENTIFIER ')' func_body
 *    ;
 */
static inline void traverse_final_except_clause(ast_final_except_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_FINAL_EXCEPT_CLAUSE);

    TRACE_TOKEN(IDENTIFIER);
    TRAVERSE(func_body);

    AST_RETURN;
}

/*
 * except_clause
 *    : except_clause_list
 *    | except_clause_list final_except_clause
 *    | final_except_clause
 *    ;
 */
static inline void traverse_except_clause(ast_except_clause_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXCEPT_CLAUSE);

    TRAVERSE(except_clause_list);
    TRAVERSE(final_except_clause);

    AST_RETURN;
}

/*
 * The identifier is a reference to an exception ID and the string is
 * assigned to the second identifier in an except clause.
 *
 * raise_statement
 *    : RAISE '(' IDENTIFIER ',' formatted_string ')'
 *    ;
 */
static inline void traverse_raise_statement(ast_raise_statement_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_RAISE_STATEMENT);

    TRACE_TOKEN(IDENTIFIER);
    TRAVERSE(formatted_string);

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

    TRAVERSE(type_name);

    SWITCH {
        CASE(AST_EXPRESSION, expression);
        CASE(AST_LIST_INIT, list_init);
        CASE(AST_DICT_INIT, dict_init);
        DEFAULT;
    }

    AST_RETURN;
}

/*
 * list_init
 *    : '[' expression_list ']'
 *    ;
 */
static inline void traverse_list_init(ast_list_init_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_LIST_INIT);

    TRAVERSE(expression_list);

    AST_RETURN;
}

/*
 * dict_init_item
 *    : STRING_LIT ':' expression
 *    ;
 */
static inline void traverse_dict_init_item(ast_dict_init_item_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_DICT_INIT_ITEM);

    TRACE_TOKEN(STRING_LIT);
    TRAVERSE(expression);

    AST_RETURN;
}

/*
 * dict_init_item_list
 *    : dict_init_item
 *    | dict_init_item_list ',' dict_init_item
 *    ;
 */
static inline void traverse_dict_init_item_list(ast_dict_init_item_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_DICT_INIT_ITEM_LIST);

    TRAVERSE_LIST(dict_init_item);

    AST_RETURN;
}

/*
 * dict_init
 *    : '[' dict_init_item_list ']'
 *    ;
 */
static inline void traverse_dict_init(ast_dict_init_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_DICT_INIT);

    TRAVERSE(dict_init_item_list);

    AST_RETURN;
}

/*
 * Identifier is a reference that has to be examined when the variable
 * references are verified.
 *
 * assignment_left
 *    : IDENTIFIER
 *    | list_reference
 *    ;
 */
static inline void traverse_assignment_left(ast_assignment_left_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_ASSIGNMENT_LEFT);

    TRACE_TOKEN(IDENTIFIER);
    TRAVERSE(list_reference);

    AST_RETURN;
}

/*
 * assignment
 *    : assignment_left '=' assignment_right
 *    ;
 */
static inline void traverse_assignment(ast_assignment_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_ASSIGNMENT);

    TRAVERSE(assignment_left);
    TRAVERSE(assignment_right);

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
 *    | list_reference
 *    | func_reference
 *    ;
 */
static inline void traverse_expr_primary(ast_expr_primary_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_EXPR_PRIMARY);

    SWITCH {
        CASE(AST_RAW_VALUE, raw_value);
        CASE(AST_FORMATTED_STRING, formatted_string);
        CASE(AST_LIST_REFERENCE, list_reference);
        CASE(AST_FUNC_REFERENCE, func_reference);
        DEFAULT;
    }

    AST_RETURN;
}

/*
 * This includes hashes as well as lists, since they are the same syntax.
 *
 * list_reference
 *    : IDENTIFIER list_ref_param_list
 *    ;
 */
static inline void traverse_list_reference(ast_list_reference_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_LIST_REFERENCE);

    TRACE_TOKEN(IDENTIFIER);
    TRAVERSE(list_ref_param_list);

    AST_RETURN;
}

/*
 * These have to be throurally check for symantic correctness.
 *
 * list_ref_value
 *    : IDENTIFIER
 *    | INTEGER_LIT
 *    | STRING_LIT
 *    | list_reference
 *    | func_reference
 *    | list_ref_param
 *    ;
 */
static inline void traverse_list_ref_value(ast_list_ref_value_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_LIST_REF_VALUE);

    if(node->token != NULL) {
        TRACE_TOKEN(token);
    }
    else {
        SWITCH {
            CASE(AST_LIST_REFERENCE, list_reference);
            CASE(AST_FUNC_REFERENCE, func_reference);
            CASE(AST_LIST_REF_PARAM, list_ref_param);
            DEFAULT;
        }
    }

    AST_RETURN;
}

/*
 * list_ref_param
 *    : '[' list_ref_value ']'
 *    ;
 */
static inline void traverse_list_ref_param(ast_list_ref_param_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_LIST_REF_PARAM);

    TRAVERSE(list_ref_value);

    AST_RETURN;
}

/*
 * list_ref_param_list
 *    : list_ref_param
 *    | list_ref_param_list list_ref_param
 *    ;
 */
static inline void traverse_list_ref_param_list(ast_list_ref_param_list_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    AST_ENTER(AST_LIST_REF_PARAM_LIST);

    TRAVERSE_LIST(list_ref_param);

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

    if(node->primary != NULL)
        traverse_expr_primary(node->primary, pre, post);
    else if(node->expr != NULL)
        traverse_expression(node->expr, pre, post);
    else if(node->oper->type == NOT_OPER) {
        TRACE_TOKEN(oper);
        traverse_expression(node->right, pre, post);
    }
    else if(node->oper->type == SUB_OPER && node->left == NULL) {
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


void traverse_ast(void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER;

    ASSERT(root_node != NULL, "root node is NULL!");
    traverse_program(root_node, pre, post);

    RETURN();
}

const char* node_type_to_str(ast_node_t* node) {

    ast_type_t type = node->type;

    return (type == AST_PROGRAM)                ? "AST_PROGRAM" :
            (type == AST_PROGRAM_ITEM_LIST)     ? "AST_PROGRAM_ITEM_LIST" :
            (type == AST_PROGRAM_ITEM)          ? "AST_PROGRAM_ITEM" :
            (type == AST_TYPE_NAME)             ? "AST_TYPE_NAME" :
            (type == AST_FORMATTED_STRING)      ? "AST_FORMATTED_STRING" :
            (type == AST_IMPORT_STATEMENT)      ? "AST_IMPORT_STATEMENT" :
            (type == AST_DATA_DECLARATION)      ? "AST_DATA_DECLARATION" :
            (type == AST_DATA_DECLARATION_LIST) ? "AST_DATA_DECLARATION_LIST" :
            (type == AST_DATA_DEFINITION)       ? "AST_DATA_DEFINITION" :
            (type == AST_FUNC_DEFINITION)       ? "AST_FUNC_DEFINITION" :
            (type == AST_FUNC_PARAMS)           ? "AST_FUNC_PARAMS" :
            (type == AST_FUNC_BODY)             ? "AST_FUNC_BODY" :
            (type == AST_FUNC_NAME)             ? "AST_FUNC_NAME" :
            (type == AST_FUNC_BODY_LIST)        ? "AST_FUNC_BODY_LIST" :
            (type == AST_LOOP_BODY)             ? "AST_LOOP_BODY" :
            (type == AST_LOOP_BODY_LIST)        ? "AST_LOOP_BODY_LIST" :
            (type == AST_LOOP_BODY_DIFFS)       ? "AST_LOOP_BODY_DIFFS" :
            (type == AST_LOOP_BODY_ELEM)        ? "AST_LOOP_BODY_ELEM" :
            (type == AST_FUNC_BODY_ELEM)        ? "AST_FUNC_BODY_ELEM" :
            (type == AST_TRACE_STATEMENT)       ? "AST_TRACE_STATEMENT" :
            (type == AST_PRINT_STATEMENT)       ? "AST_PRINT_STATEMENT" :
            (type == AST_FOR_STATEMENT)         ? "AST_FOR_STATEMENT" :
            (type == AST_EXIT_STATEMENT)        ? "AST_EXIT_STATEMENT" :
            (type == AST_RETURN_STATEMENT)      ? "AST_RETURN_STATEMENT" :
            (type == AST_TRYEXCEPT_STATEMENT)   ? "AST_TRYEXCEPT_STATEMENT" :
            (type == AST_TRY_CLAUSE)            ? "AST_TRY_CLAUSE" :
            (type == AST_EXCEPT_SEGMENT)        ? "AST_EXCEPT_SEGMENT" :
            (type == AST_EXCEPT_CLAUSE_LIST)    ? "AST_EXCEPT_CLAUSE_LIST" :
            (type == AST_FINAL_EXCEPT_CLAUSE)   ? "AST_FINAL_EXCEPT_CLAUSE" :
            (type == AST_EXCEPT_CLAUSE)         ? "AST_EXCEPT_CLAUSE" :
            (type == AST_RAISE_STATEMENT)       ? "AST_RAISE_STATEMENT" :
            (type == AST_IF_CLAUSE)             ? "AST_IF_CLAUSE" :
            (type == AST_IFELSE_STATEMENT)      ? "AST_IFELSE_STATEMENT" :
            (type == AST_ELSE_SEGMENT)          ? "AST_ELSE_SEGMENT" :
            (type == AST_FINAL_ELSE_SEGMENT)    ? "AST_FINAL_ELSE_SEGMENT" :
            (type == AST_ELSE_CLAUSE_LIST)      ? "AST_ELSE_CLAUSE_LIST" :
            (type == AST_ELSE_CLAUSE)           ? "AST_ELSE_CLAUSE" :
            (type == AST_WHILE_CLAUSE)          ? "AST_WHILE_CLAUSE" :
            (type == AST_WHILE_STATEMENT)       ? "AST_WHILE_STATEMENT" :
            (type == AST_DO_STATEMENT)          ? "AST_DO_STATEMENT" :
            (type == AST_ASSIGNMENT_RIGHT)      ? "AST_ASSIGNMENT_RIGHT" :
            (type == AST_ASSIGNMENT)            ? "AST_ASSIGNMENT" :
            (type == AST_RAW_VALUE)             ? "AST_RAW_VALUE" :
            (type == AST_EXPR_PRIMARY)          ? "AST_EXPR_PRIMARY" :
            (type == AST_LIST_REFERENCE)        ? "AST_LIST_REFERENCE" :
            (type == AST_LIST_REF_VALUE)        ? "AST_LIST_REF_VALUE" :
            (type == AST_LIST_REF_PARAM)        ? "AST_LIST_REF_PARAM" :
            (type == AST_LIST_REF_PARAM_LIST)   ? "AST_LIST_REF_PARAM_LIST" :
            (type == AST_FUNC_REFERENCE)        ? "AST_FUNC_REFERENCE" :
            (type == AST_EXPRESSION)            ? "AST_EXPRESSION" :
            (type == AST_EXPRESSION_LIST)       ? "AST_EXPRESSION_LIST" :
            (type == AST_EXPRESSION_LIST_PARAM) ? "AST_EXPRESSION_LIST_PARAM" :
            (type == AST_EXPRESSION_PARAM)      ? "AST_EXPRESSION_PARAM" :
                                                  "UNKNOWN";
}

const char* node_type_to_name(ast_node_t* node) {

    ast_type_t type = node->type;

    return (type == AST_PROGRAM)                ? "program" :
            (type == AST_PROGRAM_ITEM_LIST)     ? "program_item_list" :
            (type == AST_PROGRAM_ITEM)          ? "program_item" :
            (type == AST_TYPE_NAME)             ? "type_name" :
            (type == AST_FORMATTED_STRING)      ? "formatted_string" :
            (type == AST_IMPORT_STATEMENT)      ? "import_statement" :
            (type == AST_DATA_DECLARATION)      ? "data_declaration" :
            (type == AST_DATA_DECLARATION_LIST) ? "data_declaration_list" :
            (type == AST_DATA_DEFINITION)       ? "data_definition" :
            (type == AST_FUNC_DEFINITION)       ? "func_definition" :
            (type == AST_FUNC_PARAMS)           ? "func_params" :
            (type == AST_FUNC_BODY)             ? "func_body" :
            (type == AST_FUNC_NAME)             ? "func_name" :
            (type == AST_FUNC_BODY_LIST)        ? "func_body_list" :
            (type == AST_LOOP_BODY)             ? "loop_body" :
            (type == AST_LOOP_BODY_LIST)        ? "loop_body_list" :
            (type == AST_LOOP_BODY_DIFFS)       ? "loop_body_diffs" :
            (type == AST_LOOP_BODY_ELEM)        ? "loop_body_elem" :
            (type == AST_FUNC_BODY_ELEM)        ? "func_body_elem" :
            (type == AST_TRACE_STATEMENT)       ? "trace_statement" :
            (type == AST_PRINT_STATEMENT)       ? "print_statement" :
            (type == AST_FOR_STATEMENT)         ? "for_statement" :
            (type == AST_EXIT_STATEMENT)        ? "exit_statement" :
            (type == AST_RETURN_STATEMENT)      ? "return_statement" :
            (type == AST_TRYEXCEPT_STATEMENT)   ? "tryexcept_statement" :
            (type == AST_TRY_CLAUSE)            ? "try_clause" :
            (type == AST_EXCEPT_SEGMENT)        ? "except_segment" :
            (type == AST_EXCEPT_CLAUSE_LIST)    ? "except_clause_list" :
            (type == AST_FINAL_EXCEPT_CLAUSE)   ? "final_except_clause" :
            (type == AST_EXCEPT_CLAUSE)         ? "except_clause" :
            (type == AST_RAISE_STATEMENT)       ? "raise_statement" :
            (type == AST_IF_CLAUSE)             ? "if_clause" :
            (type == AST_IFELSE_STATEMENT)      ? "ifelse_statement" :
            (type == AST_ELSE_SEGMENT)          ? "else_segment" :
            (type == AST_FINAL_ELSE_SEGMENT)    ? "final_else_segment" :
            (type == AST_ELSE_CLAUSE_LIST)      ? "else_clause_list" :
            (type == AST_ELSE_CLAUSE)           ? "else_clause" :
            (type == AST_WHILE_CLAUSE)          ? "while_clause" :
            (type == AST_WHILE_STATEMENT)       ? "while_statement" :
            (type == AST_DO_STATEMENT)          ? "do_statement" :
            (type == AST_ASSIGNMENT_RIGHT)      ? "assignment_right" :
            (type == AST_ASSIGNMENT)            ? "assignment" :
            (type == AST_RAW_VALUE)             ? "raw_value" :
            (type == AST_EXPR_PRIMARY)          ? "expr_primary" :
            (type == AST_LIST_REFERENCE)        ? "list_reference" :
            (type == AST_LIST_REF_VALUE)        ? "list_ref_value" :
            (type == AST_LIST_REF_PARAM)        ? "list_ref_param" :
            (type == AST_LIST_REF_PARAM_LIST)   ? "list_ref_param_list" :
            (type == AST_FUNC_REFERENCE)        ? "func_reference" :
            (type == AST_EXPRESSION)            ? "expression" :
            (type == AST_EXPRESSION_LIST)       ? "expression_list" :
            (type == AST_EXPRESSION_LIST_PARAM) ? "expression_list_param" :
            (type == AST_EXPRESSION_PARAM)      ? "expression_param" :
                                                  "UNKNOWN";
}

size_t alloc_ast_node_size(ast_type_t type) {

    return (type == AST_PROGRAM)                ? sizeof(ast_program_t) :
            (type == AST_PROGRAM_ITEM_LIST)     ? sizeof(ast_program_item_list_t) :
            (type == AST_PROGRAM_ITEM)          ? sizeof(ast_program_item_t) :
            (type == AST_TYPE_NAME)             ? sizeof(ast_type_name_t) :
            (type == AST_FORMATTED_STRING)      ? sizeof(ast_formatted_string_t) :
            (type == AST_IMPORT_STATEMENT)      ? sizeof(ast_import_statement_t) :
            (type == AST_DATA_DECLARATION)      ? sizeof(ast_data_declaration_t) :
            (type == AST_DATA_DECLARATION_LIST) ? sizeof(ast_data_declaration_list_t) :
            (type == AST_DATA_DEFINITION)       ? sizeof(ast_data_definition_t) :
            (type == AST_FUNC_DEFINITION)       ? sizeof(ast_func_definition_t) :
            (type == AST_FUNC_PARAMS)           ? sizeof(ast_func_params_t) :
            (type == AST_FUNC_BODY)             ? sizeof(ast_func_body_t) :
            (type == AST_FUNC_NAME)             ? sizeof(ast_func_name_t) :
            (type == AST_FUNC_BODY_LIST)        ? sizeof(ast_func_body_list_t) :
            (type == AST_LOOP_BODY)             ? sizeof(ast_loop_body_t) :
            (type == AST_LOOP_BODY_LIST)        ? sizeof(ast_loop_body_list_t) :
            (type == AST_LOOP_BODY_DIFFS)       ? sizeof(ast_loop_body_diffs_t) :
            (type == AST_LOOP_BODY_ELEM)        ? sizeof(ast_loop_body_elem_t) :
            (type == AST_FUNC_BODY_ELEM)        ? sizeof(ast_func_body_elem_t) :
            (type == AST_TRACE_STATEMENT)       ? sizeof(ast_trace_statement_t) :
            (type == AST_PRINT_STATEMENT)       ? sizeof(ast_print_statement_t) :
            (type == AST_EXIT_STATEMENT)        ? sizeof(ast_exit_statement_t) :
            (type == AST_RETURN_STATEMENT)      ? sizeof(ast_return_statement_t) :
            (type == AST_TRYEXCEPT_STATEMENT)   ? sizeof(ast_tryexcept_statement_t) :
            (type == AST_TRY_CLAUSE)            ? sizeof(ast_try_clause_t) :
            (type == AST_EXCEPT_SEGMENT)        ? sizeof(ast_except_segment_t) :
            (type == AST_EXCEPT_CLAUSE_LIST)    ? sizeof(ast_except_clause_list_t) :
            (type == AST_FINAL_EXCEPT_CLAUSE)   ? sizeof(ast_final_except_clause_t) :
            (type == AST_EXCEPT_CLAUSE)         ? sizeof(ast_except_clause_t) :
            (type == AST_RAISE_STATEMENT)       ? sizeof(ast_raise_statement_t) :
            (type == AST_FOR_STATEMENT)         ? sizeof(ast_for_statement_t) :
            (type == AST_IF_CLAUSE)             ? sizeof(ast_if_clause_t) :
            (type == AST_IFELSE_STATEMENT)      ? sizeof(ast_ifelse_statement_t) :
            (type == AST_ELSE_SEGMENT)          ? sizeof(ast_else_segment_t) :
            (type == AST_FINAL_ELSE_SEGMENT)    ? sizeof(ast_final_else_segment_t) :
            (type == AST_ELSE_CLAUSE_LIST)      ? sizeof(ast_else_clause_list_t) :
            (type == AST_ELSE_CLAUSE)           ? sizeof(ast_else_clause_t) :
            (type == AST_WHILE_CLAUSE)          ? sizeof(ast_while_clause_t) :
            (type == AST_WHILE_STATEMENT)       ? sizeof(ast_while_statement_t) :
            (type == AST_DO_STATEMENT)          ? sizeof(ast_do_statement_t) :
            (type == AST_ASSIGNMENT_RIGHT)      ? sizeof(ast_assignment_right_t) :
            (type == AST_ASSIGNMENT_LEFT)       ? sizeof(ast_assignment_left_t) :
            (type == AST_ASSIGNMENT)            ? sizeof(ast_assignment_t) :
            (type == AST_RAW_VALUE)             ? sizeof(ast_raw_value_t) :
            (type == AST_EXPR_PRIMARY)          ? sizeof(ast_expr_primary_t) :
            (type == AST_LIST_REFERENCE)        ? sizeof(ast_list_reference_t) :
            (type == AST_LIST_REF_VALUE)        ? sizeof(ast_list_ref_value_t) :
            (type == AST_LIST_REF_PARAM)        ? sizeof(ast_list_ref_param_t) :
            (type == AST_LIST_REF_PARAM_LIST)   ? sizeof(ast_list_ref_param_list_t) :
            (type == AST_FUNC_REFERENCE)        ? sizeof(ast_func_reference_t) :
            (type == AST_EXPRESSION)            ? sizeof(ast_expression_t) :
            (type == AST_EXPRESSION_LIST)       ? sizeof(ast_expression_list_t) :
            (type == AST_EXPRESSION_LIST_PARAM) ? sizeof(ast_expression_list_param_t) :
            (type == AST_EXPRESSION_PARAM)      ? sizeof(ast_expression_param_t) :
            (type == AST_DICT_INIT)             ? sizeof(ast_dict_init_t) :
            (type == AST_LIST_INIT)             ? sizeof(ast_list_init_t) :
            (type == AST_DICT_INIT_ITEM)        ? sizeof(ast_dict_init_item_t) :
            (type == AST_DICT_INIT_ITEM_LIST)   ? sizeof(ast_dict_init_item_list_t) :
            (type == AST_EXCEPTION_IDENTIFIER)  ? sizeof(ast_exception_identifier_t) :
                                                  (size_t)-1;
}

ast_node_t* alloc_ast_node(ast_type_t type) {

    return (ast_node_t*)_ALLOC(alloc_ast_node_size(type));
}
