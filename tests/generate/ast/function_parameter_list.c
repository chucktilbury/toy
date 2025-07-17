/**
 *
 * @file function_parameter_list.c
 *
 * @brief Traverse AST for node function_parameter_list.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * function_parameter_list
 *     : data_declaration
 *     | function_parameter_list COMMA data_declaration
 *     ;
 */
void traverse_function_parameter_list(ast_function_parameter_list_t* node) {

    ENTER;
    RETURN();
}

