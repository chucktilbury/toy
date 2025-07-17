/**
 *
 * @file loop_body_prelist.c
 *
 * @brief Parse grammar production loop_body_prelist.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * loop_body_prelist
 *     : loop_body_element
 *     | loop_body
 *     ;
 */
ast_loop_body_prelist_t* parse_loop_body_prelist(void) {

    ENTER;

    ast_loop_body_prelist_t* node = NULL;
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
                node = (ast_loop_body_prelist_t*)create_ast_node(AST_LOOP_BODY_PRELIST);
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

