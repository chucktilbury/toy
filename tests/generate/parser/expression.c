/**
 *
 * @file expression.c
 *
 * @brief Parse grammar production expression.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

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
ast_expression_t* parse_expression(void) {

    ENTER;

    ast_expression_t* node = NULL;
    int state = STATE_START;
    bool finished = false;
    void* post = post_token_queue();

    while(!finished) {
        switch(state) {
            case STATE_START:
                // initial state
                STATE;
                break;

            case STATE_MATCH:
                // production recognized
                STATE;
                node = (ast_expression_t*)create_ast_node(AST_EXPRESSION);
                finished = true;
                break;

            case STATE_NO_MATCH:
                // not a match, not an error
                STATE;
                reset_token_queue(post);
                finished = true;
                break;

            case STATE_ERROR:
                // error found
                STATE;
                recover_error();
                finished = true;
                break;

            default:
                FATAL("unknown state: %d", state);
        }
    }

    RETURN(node);
}

