/**
 *
 * @file compound_reference.c
 *
 * @brief Traverse AST for node compound_reference.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * compound_reference
 *     : compound_reference_element
 *     | compound_reference DOT compound_reference_element
 *     ;
 */
void traverse_compound_reference(ast_compound_reference_t* node) {

    ENTER;
    RETURN();
}

