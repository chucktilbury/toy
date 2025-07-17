/**
 *
 * @file do_clause.c
 *
 * @brief Traverse AST for node do_clause.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * do_clause
 *     : DO loop_body WHILE OPAREN expression CPAREN
 *     | DO loop_body WHILE OPAREN CPAREN
 *     | DO loop_body WHILE
 *     ;
 */
void traverse_do_clause(ast_do_clause_t* node) {

    ENTER;
    RETURN();
}

