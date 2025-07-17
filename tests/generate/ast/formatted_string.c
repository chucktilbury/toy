/**
 *
 * @file formatted_string.c
 *
 * @brief Traverse AST for node formatted_string.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * formatted_string
 *     : STRING_LITERAL
 *     | STRING_LITERAL OPAREN CPAREN
 *     | STRING_LITERAL OPAREN dss_initializer CPAREN
 *     ;
 */
void traverse_formatted_string(ast_formatted_string_t* node) {

    ENTER;
    RETURN();
}

