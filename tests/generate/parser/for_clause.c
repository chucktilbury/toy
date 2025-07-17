/**
 *
 * @file for_clause.c
 *
 * @brief Parse grammar production for_clause.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * for_clause
 *     : FOR loop_body
 *     | FOR OPAREN CPAREN loop_body
 *     | FOR OPAREN expression CPAREN loop_body
 *     | FOR OPAREN IDENTIFIER IN expression CPAREN loop_body
 *     | FOR OPAREN type_name IDENTIFIER IN expression CPAREN loop_body
 *     ;
 */
ast_for_clause_t* parse_for_clause(void) {

    ENTER;

    ast_for_clause_t* node = NULL;
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
                node = (ast_for_clause_t*)create_ast_node(AST_FOR_CLAUSE);
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

