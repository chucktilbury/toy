/**
 * @file ./out/ast/dss_initializer_item.c
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
 * Traverse the dss_initializer_item data structure.
 *
 * dss_initializer_item (
 *     TOK_STRING_LITERAL TOK_COLON expression
 * )
 *
 *
 * begin grouping_function (1000)
 *   terminal rule element: TOK_STRING_LITERAL
 *   terminal rule element: TOK_COLON
 *   non-terminal rule element: expression
 * end grouping_function (1000)
 *
 */
void traverse_dss_initializer_item(ast_dss_initializer_item_t* node) {

    ENTER;
    if(node == NULL)
        RETURN();

    TRAVERSE_TOKEN(node->STRING_LITERAL);
    TRAVERSE_NODE(expression);

    RETURN();
}
