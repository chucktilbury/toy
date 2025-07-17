/**
 *
 * @file exit_statement.c
 *
 * @brief AST implementation.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include <stddef.h>

#include "ast.h"

static size_t node_size(ast_node_type_t type) {

    return
    (type == AST_TRANSLATION_UNIT)? sizeof(ast_translation_unit_t) : 
    (type == AST_TRANSLATION_UNIT_ELEMENT)? sizeof(ast_translation_unit_element_t) : 
    (type == AST_IMPORT_STATEMENT)? sizeof(ast_import_statement_t) : 
    (type == AST_DATA_DECLARATION)? sizeof(ast_data_declaration_t) : 
    (type == AST_DATA_DEFINITION)? sizeof(ast_data_definition_t) : 
    (type == AST_INITIALIZER)? sizeof(ast_initializer_t) : 
    (type == AST_LIST_INITIALIZER)? sizeof(ast_list_initializer_t) : 
    (type == AST_DSS_INITIALIZER_ITEM)? sizeof(ast_dss_initializer_item_t) : 
    (type == AST_DSS_INITIALIZER)? sizeof(ast_dss_initializer_t) : 
    (type == AST_FORMATTED_STRING)? sizeof(ast_formatted_string_t) : 
    (type == AST_TYPE_NAME)? sizeof(ast_type_name_t) : 
    (type == AST_COMPOUND_NAME)? sizeof(ast_compound_name_t) : 
    (type == AST_FUNCTION_NAME)? sizeof(ast_function_name_t) : 
    (type == AST_FUNCTION_DEFINITION)? sizeof(ast_function_definition_t) : 
    (type == AST_FUNCTION_PARAMETERS)? sizeof(ast_function_parameters_t) : 
    (type == AST_FUNCTION_PARAMETER_LIST)? sizeof(ast_function_parameter_list_t) : 
    (type == AST_STRUCT_DATA_LIST)? sizeof(ast_struct_data_list_t) : 
    (type == AST_STRUCT_BODY)? sizeof(ast_struct_body_t) : 
    (type == AST_STRUCT_DEFINITION)? sizeof(ast_struct_definition_t) : 
    (type == AST_EXPRESSION)? sizeof(ast_expression_t) : 
    (type == AST_PRIMARY_EXPRESSION)? sizeof(ast_primary_expression_t) : 
    (type == AST_EXPRESSION_LIST)? sizeof(ast_expression_list_t) : 
    (type == AST_COMPOUND_REFERENCE)? sizeof(ast_compound_reference_t) : 
    (type == AST_COMPOUND_REFERENCE_ELEMENT)? sizeof(ast_compound_reference_element_t) : 
    (type == AST_FUNCTION_REFERENCE)? sizeof(ast_function_reference_t) : 
    (type == AST_LIST_REFERENCE)? sizeof(ast_list_reference_t) : 
    (type == AST_FUNCTION_BODY_ELEMENT)? sizeof(ast_function_body_element_t) : 
    (type == AST_FUNCTION_BODY_PRELIST)? sizeof(ast_function_body_prelist_t) : 
    (type == AST_FUNCTION_BODY_LIST)? sizeof(ast_function_body_list_t) : 
    (type == AST_FUNCTION_BODY)? sizeof(ast_function_body_t) : 
    (type == AST_LOOP_BODY_ELEMENT)? sizeof(ast_loop_body_element_t) : 
    (type == AST_LOOP_BODY_PRELIST)? sizeof(ast_loop_body_prelist_t) : 
    (type == AST_LOOP_BODY_LIST)? sizeof(ast_loop_body_list_t) : 
    (type == AST_LOOP_BODY)? sizeof(ast_loop_body_t) : 
    (type == AST_ASSIGNMENT)? sizeof(ast_assignment_t) : 
    (type == AST_IF_CLAUSE)? sizeof(ast_if_clause_t) : 
    (type == AST_ELSE_CLAUSE)? sizeof(ast_else_clause_t) : 
    (type == AST_ELSE_CLAUSE_LIST)? sizeof(ast_else_clause_list_t) : 
    (type == AST_FINAL_ELSE_CLAUSE)? sizeof(ast_final_else_clause_t) : 
    (type == AST_ELSE_CLAUSE_FOLLOW)? sizeof(ast_else_clause_follow_t) : 
    (type == AST_WHILE_CLAUSE)? sizeof(ast_while_clause_t) : 
    (type == AST_DO_CLAUSE)? sizeof(ast_do_clause_t) : 
    (type == AST_FOR_CLAUSE)? sizeof(ast_for_clause_t) : 
    (type == AST_RETURN_STATEMENT)? sizeof(ast_return_statement_t) : 
    (type == AST_EXIT_STATEMENT)? sizeof(ast_exit_statement_t) : 
    (size_t)-1; // error if we reach here
}

ast_node_t* create_ast_node(ast_node_type_t type) {

    ast_node_t* node = _ALLOC(node_size(type));
    node->type = type;
    return node;
}

void traverse_ast(ast_translation_unit_t* node) {

    traverse_translation_unit(node);
}

