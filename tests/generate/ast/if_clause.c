/**
 *
 * @file if_clause.c
 *
 * @brief Traverse AST for node if_clause.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * if_clause
 *     : IF OPAREN expression CPAREN function_body
 *     | IF OPAREN expression CPAREN function_body else_clause_follow
 *     ;
 */
void traverse_if_clause(ast_if_clause_t* node) {

    ENTER;
    RETURN();
}

