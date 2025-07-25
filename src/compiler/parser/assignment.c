/**
 * @file ./out/parser/assignment.c
 *
 * This is generated by parser_template_generator. If you edit this file,
 * don't run the generator in this directory. Run it in a different one and
 * then merge the results using a tool like diff.
 *
 * @date Sat Jul 19 18:26:30 2025
 * @author Chuck Tilbury
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "trace.h"
#include "errors.h"
#include "parser_protos.h"

/*
 * assignment (
 *     compound_name TOK_EQUAL expression
 * )
 *
 *
 * begin grouping_function (1000)
 *   non-terminal rule element: compound_name
 *   terminal rule element: TOK_EQUAL
 *   non-terminal rule element: expression
 * end grouping_function (1000)
 *
 */
ast_assignment_t* parse_assignment(parser_state_t* pstate) {

    ENTER;
    ASSERT(pstate != NULL, "null pstate is not allowed");
    ast_assignment_t* retv = NULL;
    int state = 1000;
    bool finished = false;
    void* post = mark_token_queue();

    // ast_compound_name_t* compound_name = NULL;
    // token_t* TERMINAL_OPER = NULL;
    // ast_expression_t* expression = NULL;


    while(!finished) {
        switch(state) {

            // begin grouping_function rule at state 1000:0
            case 1000:
                // non-terminal rule element: compound_name
                // terminal rule element: TOK_EQUAL
                // non-terminal rule element: expression
                break;
                // end grouping_function rule at state 1000


            case STATE_MATCH:
                TRACE_STATE;
                consume_token_queue();
                retv = (ast_assignment_t*)create_ast_node(AST_ASSIGNMENT);
                // retv->compound_name = compound_name;
                // retv->TERMINAL_OPER = TERMINAL_OPER;
                // retv->expression = expression;

                break;
            case STATE_NO_MATCH:
                TRACE_STATE;
                restore_token_queue(post);
                finished = true;
                break;
            case STATE_ERROR:
                TRACE_STATE;
                restore_token_queue(post);
                recover_parser_error(pstate);
                finished = true;
                break;
            default:
                FATAL("unknown state: %d", state);
        }
    }

    RETURN(retv);
}
