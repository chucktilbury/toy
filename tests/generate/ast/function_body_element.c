/**
 *
 * @file function_body_element.c
 *
 * @brief Traverse AST for node function_body_element.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * function_body_element
 *     : assignment
 *     | compound_reference
 *     | data_definition
 *     | struct_definition
 *     | if_clause
 *     | while_clause
 *     | do_clause
 *     | for_clause
 *     | return_statement
 *     | exit_statement
 *     | INLINE
 *     ;
 */
void traverse_function_body_element(ast_function_body_element_t* node) {

    ENTER;
    RETURN();
}

