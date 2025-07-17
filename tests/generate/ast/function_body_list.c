/**
 *
 * @file function_body_list.c
 *
 * @brief Traverse AST for node function_body_list.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * function_body_list
 *     : function_body_prelist
 *     | function_body_list function_body_prelist
 *     ;
 */
void traverse_function_body_list(ast_function_body_list_t* node) {

    ENTER;
    RETURN();
}

