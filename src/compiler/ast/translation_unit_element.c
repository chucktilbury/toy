/**
 * @file ./out/ast/translation_unit_element.c
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
 * Traverse the translation_unit_element data structure.
 *
 * translation_unit_element (
 *     import_statement |
 *     data_definition |
 *     function_definition |
 *     struct_definition start_block
 * )
 *
 *
 * begin grouping_function (1000)
 *   begin or_function (1100)
 *     begin or_function (1200)
 *       begin or_function (1300)
 *         non-terminal rule element: import_statement
 *         non-terminal rule element: data_definition
 *       end or_function (1300)
 *       non-terminal rule element: function_definition
 *     end or_function (1200)
 *     non-terminal rule element: struct_definition
 *   end or_function (1100)
 *   non-terminal rule element: start_block
 * end grouping_function (1000)
 *
 */
void traverse_translation_unit_element(ast_translation_unit_element_t* node) {

    ENTER;
    if(node == NULL)
        RETURN();

    switch(node->nterm->type) {
        case AST_DATA_DEFINITION:
            TRAVERSE_NTERM(data_definition);
            break;
        case AST_FUNCTION_DEFINITION:
            TRAVERSE_NTERM(function_definition);
            break;
        case AST_STRUCT_DEFINITION:
            TRAVERSE_NTERM(struct_definition);
            break;
        case AST_START_BLOCK:
            TRAVERSE_NTERM(start_block);
            break;
        default:
            FATAL("internal AST error: Unknown translation_unit_element type: %d", node->nterm->type);
    }


    RETURN();
}
