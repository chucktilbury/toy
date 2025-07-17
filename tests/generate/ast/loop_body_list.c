/**
 *
 * @file loop_body_list.c
 *
 * @brief Traverse AST for node loop_body_list.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * loop_body_list
 *     : loop_body_prelist
 *     | loop_body_list loop_body_prelist
 *     ;
 */
void traverse_loop_body_list(ast_loop_body_list_t* node) {

    ENTER;
    RETURN();
}

