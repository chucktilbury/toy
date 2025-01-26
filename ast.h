#ifndef _AST_H_
#define _AST_H_

#include "hash.h"

typedef enum {
    AST_PROGRAM,
    AST_PROGRAM_ITEM_LIST,
    AST_PROGRAM_ITEM,
    AST_TYPE_NAME,
    AST_FORMATTED_STRING,
    AST_IMPORT_STATEMENT,
    AST_DATA_DECLARATION,
    AST_DATA_DECLARATION_LIST,
    AST_DATA_DEFINITION,
    AST_FUNC_DEFINITION,
    AST_FUNC_PARAMS,
    AST_FUNC_BODY,
    AST_FUNC_NAME,
    AST_FUNC_BODY_LIST,
    AST_LOOP_BODY,
    AST_LOOP_BODY_LIST,
    AST_LOOP_BODY_DIFFS,
    AST_LOOP_BODY_ELEM,
    AST_FUNC_BODY_ELEM,
    AST_TRACE_STATEMENT,
    AST_PRINT_STATEMENT,
    AST_EXIT_STATEMENT,
    AST_RETURN_STATEMENT,
    AST_TRYEXCEPT_STATEMENT,
    AST_TRY_CLAUSE,
    AST_EXCEPT_SEGMENT,
    AST_EXCEPT_CLAUSE_LIST,
    AST_FINAL_EXCEPT_CLAUSE,
    AST_EXCEPT_CLAUSE,
    AST_RAISE_STATEMENT,
    AST_IF_CLAUSE,
    AST_IFELSE_STATEMENT,
    AST_ELSE_SEGMENT,
    AST_FINAL_ELSE_SEGMENT,
    AST_ELSE_CLAUSE_LIST,
    AST_ELSE_CLAUSE,
    AST_WHILE_CLAUSE,
    AST_WHILE_STATEMENT,
    AST_DO_STATEMENT,
    AST_ASSIGNMENT_RIGHT,
    AST_ASSIGNMENT_LEFT,
    AST_ASSIGNMENT,
    AST_RAW_VALUE,
    AST_EXPR_PRIMARY,
    AST_LIST_REFERENCE,
    AST_LIST_REF_VALUE,
    AST_LIST_REF_PARAM,
    AST_LIST_REF_PARAM_LIST,
    AST_FUNC_REFERENCE,
    AST_EXPRESSION,
    AST_EXPRESSION_LIST,
    AST_EXPRESSION_LIST_PARAM,
    AST_EXPRESSION_PARAM,
    AST_DICT_INIT,
    AST_LIST_INIT,
    AST_DICT_INIT_ITEM,
    AST_DICT_INIT_ITEM_LIST,
} ast_type_t;

typedef struct {
    ast_type_t type;
    hash_table_t* table;
} ast_node_t;

ast_node_t* create_ast_node(ast_type_t type);
void add_ast_node_attrib(ast_node_t* node, const char* key, void* attrib);
int get_ast_node_attrib(ast_node_t* node, const char* key, ast_node_t** val);
void add_ast_node_list_item(ast_node_t* node, const char* key, void* attrib);
void traverse_ast(void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
const char* node_type_to_str(ast_node_t* node);
const char* node_type_to_name(ast_node_t* node);

#endif /* _AST_H_ */
