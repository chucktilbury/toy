/**
 *
 * @file function_body_element.c
 *
 * @brief Parse grammar production function_body_element.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "tokens.h"
#include "trace.h"
#include "parser.h"

/**
 * function_body_element
 *     : assignment
 *     | compound_reference
 *     | data_definition
 *     | struct_definition
 *     | if_clause
 *     | while_clause
 *     | do_clause
 *     | for_clause
 *     | return_statement
 *     | exit_statement
 *     | INLINE
 *     ;
 */
ast_function_body_element_t* parse_function_body_element(void) {

    ENTER;

    ast_function_body_element_t* node = NULL;
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
                node = (ast_function_body_element_t*)create_ast_node(AST_FUNCTION_BODY_ELEMENT);
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

