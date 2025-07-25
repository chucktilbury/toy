/**
 * @file ./out/ast/formatted_string.c
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
 * Traverse the formatted_string data structure.
 *
 * formatted_string (
 *     TOK_STRING_LITERAL ( TOK_OPAREN dss_initializer ? TOK_CPAREN ) ?
 * )
 *
 *
 * begin grouping_function (1000)
 *   terminal rule element: TOK_STRING_LITERAL
 *   begin zero_or_one_function (1100)
 *     begin grouping_function (1200)
 *       terminal rule element: TOK_OPAREN
 *       begin zero_or_one_function (1300)
 *         non-terminal rule element: dss_initializer
 *       end zero_or_one_function (1300)
 *       terminal rule element: TOK_CPAREN
 *     end grouping_function (1200)
 *   end zero_or_one_function (1100)
 * end grouping_function (1000)
 *
 */
void traverse_formatted_string(ast_formatted_string_t* node) {

    ENTER;
    if(node == NULL)
        RETURN();

    TRAVERSE_TOKEN(node->STRING_LITERAL);
    TRAVERSE_NODE(dss_initializer);

    RETURN();
}
