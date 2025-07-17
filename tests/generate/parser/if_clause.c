/**
 *
 * @file if_clause.c
 *
 * @brief Parse grammar production if_clause.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * if_clause
 *     : IF OPAREN expression CPAREN function_body
 *     | IF OPAREN expression CPAREN function_body else_clause_follow
 *     ;
 */
ast_if_clause_t* parse_if_clause(void) {

    ENTER;

    ast_if_clause_t* node = NULL;
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
                node = (ast_if_clause_t*)create_ast_node(AST_IF_CLAUSE);
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

