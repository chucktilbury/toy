/**
 *
 * @file primary_expression.c
 *
 * @brief Traverse AST for node primary_expression.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * primary_expression
 *     : INT_LITERAL
 *     | FLOAT_LITERAL
 *     | formatted_string
 *     | OPAREN expression CPAREN
 *     | compound_reference
 *     ;
 */
void traverse_primary_expression(ast_primary_expression_t* node) {

    ENTER;
    RETURN();
}

