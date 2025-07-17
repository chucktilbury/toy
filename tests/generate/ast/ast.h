/**
 *
 * @file ast.h
 *
 * @brief AST traverse public interface.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */

#ifndef _AST_H_
#define _AST_H_

typedef enum {
    AST_TRANSLATION_UNIT,
    AST_TRANSLATION_UNIT_ELEMENT,
    AST_IMPORT_STATEMENT,
    AST_DATA_DECLARATION,
    AST_DATA_DEFINITION,
    AST_INITIALIZER,
    AST_LIST_INITIALIZER,
    AST_DSS_INITIALIZER_ITEM,
    AST_DSS_INITIALIZER,
    AST_FORMATTED_STRING,
    AST_TYPE_NAME,
    AST_COMPOUND_NAME,
    AST_FUNCTION_NAME,
    AST_FUNCTION_DEFINITION,
    AST_FUNCTION_PARAMETERS,
    AST_FUNCTION_PARAMETER_LIST,
    AST_STRUCT_DATA_LIST,
    AST_STRUCT_BODY,
    AST_STRUCT_DEFINITION,
    AST_EXPRESSION,
    AST_PRIMARY_EXPRESSION,
    AST_EXPRESSION_LIST,
    AST_COMPOUND_REFERENCE,
    AST_COMPOUND_REFERENCE_ELEMENT,
    AST_FUNCTION_REFERENCE,
    AST_LIST_REFERENCE,
    AST_FUNCTION_BODY_ELEMENT,
    AST_FUNCTION_BODY_PRELIST,
    AST_FUNCTION_BODY_LIST,
    AST_FUNCTION_BODY,
    AST_LOOP_BODY_ELEMENT,
    AST_LOOP_BODY_PRELIST,
    AST_LOOP_BODY_LIST,
    AST_LOOP_BODY,
    AST_ASSIGNMENT,
    AST_IF_CLAUSE,
    AST_ELSE_CLAUSE,
    AST_ELSE_CLAUSE_LIST,
    AST_FINAL_ELSE_CLAUSE,
    AST_ELSE_CLAUSE_FOLLOW,
    AST_WHILE_CLAUSE,
    AST_DO_CLAUSE,
    AST_FOR_CLAUSE,
    AST_RETURN_STATEMENT,
    AST_EXIT_STATEMENT,
} ast_node_type_t;

typedef struct _ast_node_ {
    ast_node_type_t type;
} ast_node_t;

/**
 * translation_unit
 *     : translation_unit_element
 *     | translation_unit translation_unit_element
 *     ;
 */
typedef struct _ast_translation_unit_t_ {
    ast_node_t node;

} ast_translation_unit_t;

/**
 * translation_unit_element
 *     : import_statement
 *     | data_definition
 *     | function_definition
 *     | struct_definition
 *     ;
 */
typedef struct _ast_translation_unit_element_t_ {
    ast_node_t node;

} ast_translation_unit_element_t;

/**
 * import_statement
 *     : IMPORT STRING_LITERAL
 *     ;
 */
typedef struct _ast_import_statement_t_ {
    ast_node_t node;

} ast_import_statement_t;

/**
 * data_declaration
 *     : type_name IDENTIFIER
 *     ;
 */
typedef struct _ast_data_declaration_t_ {
    ast_node_t node;

} ast_data_declaration_t;

/**
 * data_definition
 *     : data_declaration
 *     | data_declaration EQUAL initializer
 *     | CONST data_declaration EQUAL initializer
 *     ;
 */
typedef struct _ast_data_definition_t_ {
    ast_node_t node;

} ast_data_definition_t;

/**
 * initializer
 *     : expression
 *     | OSBRACE list_initializer CSBRACE
 *     | OSBRACE dss_initializer CSBRACE
 *     | OCBRACE dss_initializer CCBRACE
 *     ;
 */
typedef struct _ast_initializer_t_ {
    ast_node_t node;

} ast_initializer_t;

/**
 * list_initializer
 *     : expression
 *     | list_initializer COMMA expression
 *     ;
 */
typedef struct _ast_list_initializer_t_ {
    ast_node_t node;

} ast_list_initializer_t;

/**
 * dss_initializer_item
 *     : STRING_LITERAL COLON expression
 *     ;
 */
typedef struct _ast_dss_initializer_item_t_ {
    ast_node_t node;

} ast_dss_initializer_item_t;

/**
 * dss_initializer
 *     : dss_initializer_item
 *     | dss_initializer COMMA dss_initializer_item
 *     ;
 */
typedef struct _ast_dss_initializer_t_ {
    ast_node_t node;

} ast_dss_initializer_t;

/**
 * formatted_string
 *     : STRING_LITERAL
 *     | STRING_LITERAL OPAREN CPAREN
 *     | STRING_LITERAL OPAREN dss_initializer CPAREN
 *     ;
 */
typedef struct _ast_formatted_string_t_ {
    ast_node_t node;

} ast_formatted_string_t;

/**
 * type_name
 *     : INT
 *     | FLOAT
 *     | STRING
 *     | LIST
 *     | DICT
 *     | BOOL
 *     | compound_name
 *     ;
 */
typedef struct _ast_type_name_t_ {
    ast_node_t node;

} ast_type_name_t;

/**
 * compound_name
 *     : IDENTIFIER
 *     | compound_name DOT IDENTIFIER
 *     ;
 */
typedef struct _ast_compound_name_t_ {
    ast_node_t node;

} ast_compound_name_t;

/**
 * function_name
 *     : type_name IDENTIFIER
 *     | NOTHING IDENTIFIER
 *     ;
 */
typedef struct _ast_function_name_t_ {
    ast_node_t node;

} ast_function_name_t;

/**
 * function_definition
 *     : function_name function_parameters function_body
 *     ;
 */
typedef struct _ast_function_definition_t_ {
    ast_node_t node;

} ast_function_definition_t;

/**
 * function_parameters
 *     : OPAREN function_parameter_list CPAREN
 *     | OPAREN CPAREN
 *     ;
 */
typedef struct _ast_function_parameters_t_ {
    ast_node_t node;

} ast_function_parameters_t;

/**
 * function_parameter_list
 *     : data_declaration
 *     | function_parameter_list COMMA data_declaration
 *     ;
 */
typedef struct _ast_function_parameter_list_t_ {
    ast_node_t node;

} ast_function_parameter_list_t;

/**
 * struct_data_list
 *     : data_definition
 *     | struct_data_list data_definition
 *     ;
 */
typedef struct _ast_struct_data_list_t_ {
    ast_node_t node;

} ast_struct_data_list_t;

/**
 * struct_body
 *     : OCBRACE struct_data_list CCBRACE
 *     ;
 */
typedef struct _ast_struct_body_t_ {
    ast_node_t node;

} ast_struct_body_t;

/**
 * struct_definition
 *     : STRUCT IDENTIFIER struct_body
 *     ;
 */
typedef struct _ast_struct_definition_t_ {
    ast_node_t node;

} ast_struct_definition_t;

/**
 * expression
 *     : expression STAR expression
 *     | expression SLASH expression
 *     | expression PERCENT expression
 *     | expression PLUS expression
 *     | expression MINUS expression
 *     | expression GT expression
 *     | expression LT expression
 *     | expression GTE expression
 *     | expression LTE expression
 *     | expression EQU expression
 *     | expression NEQU expression
 *     | expression AND expression
 *     | expression OR expression
 *     | expression CARAT expression
 *     | NOT expression %prec UNARY
 *     | MINUS expression %prec UNARY
 *     | primary_expression
 *     ;
 */
typedef struct _ast_expression_t_ {
    ast_node_t node;

} ast_expression_t;

/**
 * primary_expression
 *     : INT_LITERAL
 *     | FLOAT_LITERAL
 *     | formatted_string
 *     | OPAREN expression CPAREN
 *     | compound_reference
 *     ;
 */
typedef struct _ast_primary_expression_t_ {
    ast_node_t node;

} ast_primary_expression_t;

/**
 * expression_list
 *     : expression
 *     | expression_list COMMA expression
 *     ;
 */
typedef struct _ast_expression_list_t_ {
    ast_node_t node;

} ast_expression_list_t;

/**
 * compound_reference
 *     : compound_reference_element
 *     | compound_reference DOT compound_reference_element
 *     ;
 */
typedef struct _ast_compound_reference_t_ {
    ast_node_t node;

} ast_compound_reference_t;

/**
 * compound_reference_element
 *     : IDENTIFIER
 *     | function_reference
 *     | list_reference
 *     ;
 */
typedef struct _ast_compound_reference_element_t_ {
    ast_node_t node;

} ast_compound_reference_element_t;

/**
 * function_reference
 *     : IDENTIFIER OPAREN expression_list CPAREN
 *     ;
 */
typedef struct _ast_function_reference_t_ {
    ast_node_t node;

} ast_function_reference_t;

/**
 * list_reference
 *     : IDENTIFIER list_ref_parms
 *     : OSBRACE expression CSBRACE
 *     | list_ref_parms OSBRACE expression CSBRACE
 *     ;
 */
typedef struct _ast_list_reference_t_ {
    ast_node_t node;

} ast_list_reference_t;

/**
 * function_body_element
 *     : assignment
 *     | compound_reference
 *     | data_definition
 *     | struct_definition
 *     | if_clause
 *     | while_clause
 *     | do_clause
 *     | for_clause
 *     | return_statement
 *     | exit_statement
 *     | INLINE
 *     ;
 */
typedef struct _ast_function_body_element_t_ {
    ast_node_t node;

} ast_function_body_element_t;

/**
 * function_body_prelist
 *     : function_body_element
 *     | function_body
 *     ;
 */
typedef struct _ast_function_body_prelist_t_ {
    ast_node_t node;

} ast_function_body_prelist_t;

/**
 * function_body_list
 *     : function_body_prelist
 *     | function_body_list function_body_prelist
 *     ;
 */
typedef struct _ast_function_body_list_t_ {
    ast_node_t node;

} ast_function_body_list_t;

/**
 * function_body
 *     : OCBRACE function_body_list CCBRACE
 *     ;
 */
typedef struct _ast_function_body_t_ {
    ast_node_t node;

} ast_function_body_t;

/**
 * loop_body_element
 *     : function_body_element
 *     | CONTINUE
 *     | BREAK
 *     ;
 */
typedef struct _ast_loop_body_element_t_ {
    ast_node_t node;

} ast_loop_body_element_t;

/**
 * loop_body_prelist
 *     : loop_body_element
 *     | loop_body
 *     ;
 */
typedef struct _ast_loop_body_prelist_t_ {
    ast_node_t node;

} ast_loop_body_prelist_t;

/**
 * loop_body_list
 *     : loop_body_prelist
 *     | loop_body_list loop_body_prelist
 *     ;
 */
typedef struct _ast_loop_body_list_t_ {
    ast_node_t node;

} ast_loop_body_list_t;

/**
 * loop_body
 *     : OCBRACE loop_body_list CCBRACE
 *     ;
 */
typedef struct _ast_loop_body_t_ {
    ast_node_t node;

} ast_loop_body_t;

/**
 * assignment
 *     : compound_name EQUAL expression
 *     ;
 */
typedef struct _ast_assignment_t_ {
    ast_node_t node;

} ast_assignment_t;

/**
 * if_clause
 *     : IF OPAREN expression CPAREN function_body
 *     | IF OPAREN expression CPAREN function_body else_clause_follow
 *     ;
 */
typedef struct _ast_if_clause_t_ {
    ast_node_t node;

} ast_if_clause_t;

/**
 * else_clause
 *     : ELSE OPAREN expression CPAREN function_body
 *     ;
 */
typedef struct _ast_else_clause_t_ {
    ast_node_t node;

} ast_else_clause_t;

/**
 * else_clause_list
 *     : else_clause
 *     | else_clause_list else_clause
 *     ;
 */
typedef struct _ast_else_clause_list_t_ {
    ast_node_t node;

} ast_else_clause_list_t;

/**
 * final_else_clause
 *     : ELSE OPAREN CPAREN function_body
 *     | ELSE function_body
 *     ;
 */
typedef struct _ast_final_else_clause_t_ {
    ast_node_t node;

} ast_final_else_clause_t;

/**
 * else_clause_follow
 *     : else_clause_list final_else_clause
 *     | final_else_clause
 *     ;
 */
typedef struct _ast_else_clause_follow_t_ {
    ast_node_t node;

} ast_else_clause_follow_t;

/**
 * while_clause
 *     : WHILE OPAREN expression CPAREN loop_body
 *     | WHILE OPAREN CPAREN loop_body
 *     | WHILE loop_body
 *     ;
 */
typedef struct _ast_while_clause_t_ {
    ast_node_t node;

} ast_while_clause_t;

/**
 * do_clause
 *     : DO loop_body WHILE OPAREN expression CPAREN
 *     | DO loop_body WHILE OPAREN CPAREN
 *     | DO loop_body WHILE
 *     ;
 */
typedef struct _ast_do_clause_t_ {
    ast_node_t node;

} ast_do_clause_t;

/**
 * for_clause
 *     : FOR loop_body
 *     | FOR OPAREN CPAREN loop_body
 *     | FOR OPAREN expression CPAREN loop_body
 *     | FOR OPAREN IDENTIFIER IN expression CPAREN loop_body
 *     | FOR OPAREN type_name IDENTIFIER IN expression CPAREN loop_body
 *     ;
 */
typedef struct _ast_for_clause_t_ {
    ast_node_t node;

} ast_for_clause_t;

/**
 * return_statement
 *     : RETURN
 *     | RETURN OPAREN CPAREN
 *     | RETURN OPAREN expression CPAREN
 *     ;
 */
typedef struct _ast_return_statement_t_ {
    ast_node_t node;

} ast_return_statement_t;

/**
 * exit_statement
 *     : EXIT OPAREN expression CPAREN
 *     ;
 */
typedef struct _ast_exit_statement_t_ {
    ast_node_t node;

} ast_exit_statement_t;

void traverse_translation_unit(ast_translation_unit_t* node);
void traverse_translation_unit_element(ast_translation_unit_element_t* node);
void traverse_import_statement(ast_import_statement_t* node);
void traverse_data_declaration(ast_data_declaration_t* node);
void traverse_data_definition(ast_data_definition_t* node);
void traverse_initializer(ast_initializer_t* node);
void traverse_list_initializer(ast_list_initializer_t* node);
void traverse_dss_initializer_item(ast_dss_initializer_item_t* node);
void traverse_dss_initializer(ast_dss_initializer_t* node);
void traverse_formatted_string(ast_formatted_string_t* node);
void traverse_type_name(ast_type_name_t* node);
void traverse_compound_name(ast_compound_name_t* node);
void traverse_function_name(ast_function_name_t* node);
void traverse_function_definition(ast_function_definition_t* node);
void traverse_function_parameters(ast_function_parameters_t* node);
void traverse_function_parameter_list(ast_function_parameter_list_t* node);
void traverse_struct_data_list(ast_struct_data_list_t* node);
void traverse_struct_body(ast_struct_body_t* node);
void traverse_struct_definition(ast_struct_definition_t* node);
void traverse_expression(ast_expression_t* node);
void traverse_primary_expression(ast_primary_expression_t* node);
void traverse_expression_list(ast_expression_list_t* node);
void traverse_compound_reference(ast_compound_reference_t* node);
void traverse_compound_reference_element(ast_compound_reference_element_t* node);
void traverse_function_reference(ast_function_reference_t* node);
void traverse_list_reference(ast_list_reference_t* node);
void traverse_function_body_element(ast_function_body_element_t* node);
void traverse_function_body_prelist(ast_function_body_prelist_t* node);
void traverse_function_body_list(ast_function_body_list_t* node);
void traverse_function_body(ast_function_body_t* node);
void traverse_loop_body_element(ast_loop_body_element_t* node);
void traverse_loop_body_prelist(ast_loop_body_prelist_t* node);
void traverse_loop_body_list(ast_loop_body_list_t* node);
void traverse_loop_body(ast_loop_body_t* node);
void traverse_assignment(ast_assignment_t* node);
void traverse_if_clause(ast_if_clause_t* node);
void traverse_else_clause(ast_else_clause_t* node);
void traverse_else_clause_list(ast_else_clause_list_t* node);
void traverse_final_else_clause(ast_final_else_clause_t* node);
void traverse_else_clause_follow(ast_else_clause_follow_t* node);
void traverse_while_clause(ast_while_clause_t* node);
void traverse_do_clause(ast_do_clause_t* node);
void traverse_for_clause(ast_for_clause_t* node);
void traverse_return_statement(ast_return_statement_t* node);
void traverse_exit_statement(ast_exit_statement_t* node);

ast_node_t* create_ast_node(ast_node_type_t type);
void traverse_ast(ast_translation_unit_t* node);


#endif /* _AST_H_ */

