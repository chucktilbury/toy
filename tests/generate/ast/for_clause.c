/**
 *
 * @file for_clause.c
 *
 * @brief Traverse AST for node for_clause.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * for_clause
 *     : FOR loop_body
 *     | FOR OPAREN CPAREN loop_body
 *     | FOR OPAREN expression CPAREN loop_body
 *     | FOR OPAREN IDENTIFIER IN expression CPAREN loop_body
 *     | FOR OPAREN type_name IDENTIFIER IN expression CPAREN loop_body
 *     ;
 */
void traverse_for_clause(ast_for_clause_t* node) {

    ENTER;
    RETURN();
}

