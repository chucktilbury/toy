/**
 * @file ./out/ast/expression_list.c
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
#include "ast_protos.h"

/*
 * Traverse the expression_list data structure.
 *
 * expression_list (
 *     expression ( TOK_COMMA expression ) *
 * )
 *
 *
 * begin grouping_function (1000)
 *   non-terminal rule element: expression
 *   begin zero_or_more_function (1100)
 *     begin grouping_function (1200)
 *       terminal rule element: TOK_COMMA
 *       non-terminal rule element: expression
 *     end grouping_function (1200)
 *   end zero_or_more_function (1100)
 * end grouping_function (1000)
 *
 */
void traverse_expression_list(ast_expression_list_t* node) {

    ENTER;
    if(node == NULL)
        RETURN();

    TRAVERSE_LIST(expression);

    RETURN();
}
