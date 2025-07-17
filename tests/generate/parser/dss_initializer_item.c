/**
 *
 * @file dss_initializer_item.c
 *
 * @brief Parse grammar production dss_initializer_item.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * dss_initializer_item
 *     : STRING_LITERAL COLON expression
 *     ;
 */
ast_dss_initializer_item_t* parse_dss_initializer_item(void) {

    ENTER;

    ast_dss_initializer_item_t* node = NULL;
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
                node = (ast_dss_initializer_item_t*)create_ast_node(AST_DSS_INITIALIZER_ITEM);
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

