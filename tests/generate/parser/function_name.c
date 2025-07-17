/**
 *
 * @file function_name.c
 *
 * @brief Parse grammar production function_name.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * function_name
 *     : type_name IDENTIFIER
 *     | NOTHING IDENTIFIER
 *     ;
 */
ast_function_name_t* parse_function_name(void) {

    ENTER;

    ast_function_name_t* node = NULL;
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
                node = (ast_function_name_t*)create_ast_node(AST_FUNCTION_NAME);
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

