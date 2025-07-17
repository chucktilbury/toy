/**
 *
 * @file list_initializer.c
 *
 * @brief Traverse AST for node list_initializer.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * list_initializer
 *     : expression
 *     | list_initializer COMMA expression
 *     ;
 */
void traverse_list_initializer(ast_list_initializer_t* node) {

    ENTER;
    RETURN();
}

