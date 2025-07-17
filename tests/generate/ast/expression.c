/**
 *
 * @file expression.c
 *
 * @brief Traverse AST for node expression.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * expression
 *     : expression STAR expression
 *     | expression SLASH expression
 *     | expression PERCENT expression
 *     | expression PLUS expression
 *     | expression MINUS expression
 *     | expression GT expression
 *     | expression LT expression
 *     | expression GTE expression
 *     | expression LTE expression
 *     | expression EQU expression
 *     | expression NEQU expression
 *     | expression AND expression
 *     | expression OR expression
 *     | expression CARAT expression
 *     | NOT expression %prec UNARY
 *     | MINUS expression %prec UNARY
 *     | primary_expression
 *     ;
 */
void traverse_expression(ast_expression_t* node) {

    ENTER;
    RETURN();
}

