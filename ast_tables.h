#ifndef _AST_TABLES_H_
#define _AST_TABLES_H_

#include "ast.h"

const char* node_type_to_str(ast_node_t* node);
const char* node_type_to_name(ast_node_t* node);
ast_node_t* alloc_ast_node(ast_type_t type);


#endif
