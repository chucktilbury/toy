/**
 *
 * @file else_clause_list.c
 *
 * @brief Parse grammar production else_clause_list.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * else_clause_list
 *     : else_clause
 *     | else_clause_list else_clause
 *     ;
 */
ast_else_clause_list_t* parse_else_clause_list(void) {

    ENTER;

    ast_else_clause_list_t* node = NULL;
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
                node = (ast_else_clause_list_t*)create_ast_node(AST_ELSE_CLAUSE_LIST);
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

