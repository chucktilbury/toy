/**
 *
 * @file function_reference.c
 *
 * @brief Traverse AST for node function_reference.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * function_reference
 *     : IDENTIFIER OPAREN expression_list CPAREN
 *     ;
 */
void traverse_function_reference(ast_function_reference_t* node) {

    ENTER;
    RETURN();
}

