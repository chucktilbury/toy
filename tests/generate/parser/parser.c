/**
 *
 * @file parser.c
 *
 * @brief Parser external interface implementation.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "parser_prototypes.h"

ast_translation_unit_t* parse(void) {

    ast_translation_unit_t* translation_unit = parse_translation_unit();
    return translation_unit;
}

