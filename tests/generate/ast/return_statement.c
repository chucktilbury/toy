/**
 *
 * @file return_statement.c
 *
 * @brief Traverse AST for node return_statement.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * return_statement
 *     : RETURN
 *     | RETURN OPAREN CPAREN
 *     | RETURN OPAREN expression CPAREN
 *     ;
 */
void traverse_return_statement(ast_return_statement_t* node) {

    ENTER;
    RETURN();
}

