/**
 *
 * @file expression_list.c
 *
 * @brief Traverse AST for node expression_list.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * expression_list
 *     : expression
 *     | expression_list COMMA expression
 *     ;
 */
void traverse_expression_list(ast_expression_list_t* node) {

    ENTER;
    RETURN();
}

