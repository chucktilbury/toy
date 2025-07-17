/**
 *
 * @file while_clause.c
 *
 * @brief Traverse AST for node while_clause.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * while_clause
 *     : WHILE OPAREN expression CPAREN loop_body
 *     | WHILE OPAREN CPAREN loop_body
 *     | WHILE loop_body
 *     ;
 */
void traverse_while_clause(ast_while_clause_t* node) {

    ENTER;
    RETURN();
}

