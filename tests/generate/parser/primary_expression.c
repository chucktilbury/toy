/**
 *
 * @file primary_expression.c
 *
 * @brief Parse grammar production primary_expression.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * primary_expression
 *     : INT_LITERAL
 *     | FLOAT_LITERAL
 *     | formatted_string
 *     | OPAREN expression CPAREN
 *     | compound_reference
 *     ;
 */
ast_primary_expression_t* parse_primary_expression(void) {

    ENTER;

    ast_primary_expression_t* node = NULL;
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
                node = (ast_primary_expression_t*)create_ast_node(AST_PRIMARY_EXPRESSION);
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

