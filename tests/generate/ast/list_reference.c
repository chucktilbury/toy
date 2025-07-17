/**
 *
 * @file list_reference.c
 *
 * @brief Traverse AST for node list_reference.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * list_reference
 *     : IDENTIFIER list_ref_parms
 *     : OSBRACE expression CSBRACE
 *     | list_ref_parms OSBRACE expression CSBRACE
 *     ;
 */
void traverse_list_reference(ast_list_reference_t* node) {

    ENTER;
    RETURN();
}

