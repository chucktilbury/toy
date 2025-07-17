/**
 *
 * @file exit_statement.c
 *
 * @brief Parse grammar production exit_statement.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * exit_statement
 *     : EXIT OPAREN expression CPAREN
 *     ;
 */
ast_exit_statement_t* parse_exit_statement(void) {

    ENTER;

    ast_exit_statement_t* node = NULL;
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
                node = (ast_exit_statement_t*)create_ast_node(AST_EXIT_STATEMENT);
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

