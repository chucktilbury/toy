/**
 *
 * @file data_definition.c
 *
 * @brief Traverse AST for node data_definition.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#include "ast.h"
#include "trace.h"

/**
 * data_definition
 *     : data_declaration
 *     | data_declaration EQUAL initializer
 *     | CONST data_declaration EQUAL initializer
 *     ;
 */
void traverse_data_definition(ast_data_definition_t* node) {

    ENTER;
    RETURN();
}

