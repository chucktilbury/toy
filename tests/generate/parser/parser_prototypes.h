/**
 *
 * @file parser_prototypes.h
 *
 * @brief Internal prototypes for parser.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#ifndef _PARSER_PROTOTYPES_H_
#define _PARSER_PROTOTYPES_H_

#include "parser.h"
#include "ast.h"

ast_translation_unit_t* parse_translation_unit(void);
ast_translation_unit_element_t* parse_translation_unit_element(void);
ast_import_statement_t* parse_import_statement(void);
ast_data_declaration_t* parse_data_declaration(void);
ast_data_definition_t* parse_data_definition(void);
ast_initializer_t* parse_initializer(void);
ast_list_initializer_t* parse_list_initializer(void);
ast_dss_initializer_item_t* parse_dss_initializer_item(void);
ast_dss_initializer_t* parse_dss_initializer(void);
ast_formatted_string_t* parse_formatted_string(void);
ast_type_name_t* parse_type_name(void);
ast_compound_name_t* parse_compound_name(void);
ast_function_name_t* parse_function_name(void);
ast_function_definition_t* parse_function_definition(void);
ast_function_parameters_t* parse_function_parameters(void);
ast_function_parameter_list_t* parse_function_parameter_list(void);
ast_struct_data_list_t* parse_struct_data_list(void);
ast_struct_body_t* parse_struct_body(void);
ast_struct_definition_t* parse_struct_definition(void);
ast_expression_t* parse_expression(void);
ast_primary_expression_t* parse_primary_expression(void);
ast_expression_list_t* parse_expression_list(void);
ast_compound_reference_t* parse_compound_reference(void);
ast_compound_reference_element_t* parse_compound_reference_element(void);
ast_function_reference_t* parse_function_reference(void);
ast_list_reference_t* parse_list_reference(void);
ast_function_body_element_t* parse_function_body_element(void);
ast_function_body_prelist_t* parse_function_body_prelist(void);
ast_function_body_list_t* parse_function_body_list(void);
ast_function_body_t* parse_function_body(void);
ast_loop_body_element_t* parse_loop_body_element(void);
ast_loop_body_prelist_t* parse_loop_body_prelist(void);
ast_loop_body_list_t* parse_loop_body_list(void);
ast_loop_body_t* parse_loop_body(void);
ast_assignment_t* parse_assignment(void);
ast_if_clause_t* parse_if_clause(void);
ast_else_clause_t* parse_else_clause(void);
ast_else_clause_list_t* parse_else_clause_list(void);
ast_final_else_clause_t* parse_final_else_clause(void);
ast_else_clause_follow_t* parse_else_clause_follow(void);
ast_while_clause_t* parse_while_clause(void);
ast_do_clause_t* parse_do_clause(void);
ast_for_clause_t* parse_for_clause(void);
ast_return_statement_t* parse_return_statement(void);
ast_exit_statement_t* parse_exit_statement(void);

#endif /* _PARSER_PROTOTYPES_H_ */

