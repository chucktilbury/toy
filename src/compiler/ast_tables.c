/**
 * @file ast_tables.c
 * 
 * @brief Handle converting the AST type labels to different forms.
 * 
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-02-28
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include <stddef.h>
#include "ast.h"
#include "memory.h"

/**
 * @brief Convert the AST type to a string.
 * 
 * @param node 
 * @return const char* 
 */
const char* node_type_to_str(ast_node_t* node) {

    ast_type_t type = node->type;

    return (type == AST_PROGRAM)                ? "AST_PROGRAM" :
            (type == AST_PROGRAM_ITEM_LIST)     ? "AST_PROGRAM_ITEM_LIST" :
            (type == AST_PROGRAM_ITEM)          ? "AST_PROGRAM_ITEM" :
            (type == AST_START_BLOCK)           ? "AST_START_BLOCK" :
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
            (type == AST_INLINE_STATEMENT)      ? "AST_INLINE_STATEMENT" :
            (type == AST_RETURN_STATEMENT)      ? "AST_RETURN_STATEMENT" :
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
            (type == AST_FUNC_REFERENCE)        ? "AST_FUNC_REFERENCE" :
            (type == AST_EXPRESSION)            ? "AST_EXPRESSION" :
            (type == AST_EXPRESSION_LIST)       ? "AST_EXPRESSION_LIST" :
            (type == AST_EXPRESSION_LIST_PARAM) ? "AST_EXPRESSION_LIST_PARAM" :
            (type == AST_EXPRESSION_PARAM)      ? "AST_EXPRESSION_PARAM" :
                                                  "UNKNOWN";
}

/**
 * @brief Convert the AST node type to a friendly name.
 * 
 * @param node 
 * @return const char* 
 */
const char* node_type_to_name(ast_node_t* node) {

    ast_type_t type = node->type;

    return (type == AST_PROGRAM)                ? "program" :
            (type == AST_PROGRAM_ITEM_LIST)     ? "program_item_list" :
            (type == AST_PROGRAM_ITEM)          ? "program_item" :
            (type == AST_START_BLOCK)           ? "start_block" :
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
            (type == AST_INLINE_STATEMENT)      ? "inline_statement" :
            (type == AST_RETURN_STATEMENT)      ? "return_statement" :
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
            (type == AST_FUNC_REFERENCE)        ? "func_reference" :
            (type == AST_EXPRESSION)            ? "expression" :
            (type == AST_EXPRESSION_LIST)       ? "expression_list" :
            (type == AST_EXPRESSION_LIST_PARAM) ? "expression_list_param" :
            (type == AST_EXPRESSION_PARAM)      ? "expression_param" :
                                                  "UNKNOWN";
}

/**
 * @brief Get the size of the AST data structure that is specified by the type.
 * 
 * @param type 
 * @return size_t 
 */
size_t alloc_ast_node_size(ast_type_t type) {

    return (type == AST_PROGRAM)                ? sizeof(ast_program_t) :
            (type == AST_PROGRAM_ITEM_LIST)     ? sizeof(ast_program_item_list_t) :
            (type == AST_PROGRAM_ITEM)          ? sizeof(ast_program_item_t) :
            (type == AST_START_BLOCK)           ? sizeof(ast_start_block_t) :
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
            (type == AST_INLINE_STATEMENT)      ? sizeof(ast_inline_statement_t) :
            (type == AST_RETURN_STATEMENT)      ? sizeof(ast_return_statement_t) :
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
            (type == AST_ASSIGNMENT)            ? sizeof(ast_assignment_t) :
            (type == AST_RAW_VALUE)             ? sizeof(ast_raw_value_t) :
            (type == AST_EXPR_PRIMARY)          ? sizeof(ast_expr_primary_t) :
            (type == AST_FUNC_REFERENCE)        ? sizeof(ast_func_reference_t) :
            (type == AST_EXPRESSION)            ? sizeof(ast_expression_t) :
            (type == AST_EXPRESSION_LIST)       ? sizeof(ast_expression_list_t) :
            (type == AST_EXPRESSION_LIST_PARAM) ? sizeof(ast_expression_list_param_t) :
            (type == AST_EXPRESSION_PARAM)      ? sizeof(ast_expression_param_t) :
                                                  (size_t)-1;
}

/**
 * @brief Allocate an AST node based on the specified type.
 * 
 * @param type 
 * @return ast_node_t* 
 */
ast_node_t* alloc_ast_node(ast_type_t type) {

    return (ast_node_t*)_ALLOC(alloc_ast_node_size(type));
}
