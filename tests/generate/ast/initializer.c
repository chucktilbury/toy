/**
 *
 * @file initializer.c
 *
 * @brief Traverse AST for node initializer.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * initializer
 *     : expression
 *     | OSBRACE list_initializer CSBRACE
 *     | OSBRACE dss_initializer CSBRACE
 *     | OCBRACE dss_initializer CCBRACE
 *     ;
 */
void traverse_initializer(ast_initializer_t* node) {

    ENTER;
    RETURN();
}

