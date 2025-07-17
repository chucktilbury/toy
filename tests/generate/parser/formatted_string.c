/**
 *
 * @file formatted_string.c
 *
 * @brief Parse grammar production formatted_string.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * formatted_string
 *     : STRING_LITERAL
 *     | STRING_LITERAL OPAREN CPAREN
 *     | STRING_LITERAL OPAREN dss_initializer CPAREN
 *     ;
 */
ast_formatted_string_t* parse_formatted_string(void) {

    ENTER;

    ast_formatted_string_t* node = NULL;
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
                node = (ast_formatted_string_t*)create_ast_node(AST_FORMATTED_STRING);
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

