/**
 * @file symbol_table.c
 *
 * @brief Create and check the symbol table.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-02-28
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include "syntax_errors.h"
#include "errors.h"
#include "pointer_list.h"
#include "ast.h"
#include "parser.h"
#include "context.h"
#include "symbol.h"
#include "type_table.h"

context_t* ctx;

#define SYMBOLS_TRACE

#ifdef SYMBOLS_TRACE
#define USE_TRACE
#endif
#include "trace.h"

static inline void _program(ast_program_t* node);
static inline void _program_item_list(ast_program_item_list_t* node);
static inline void _import_statement(ast_import_statement_t* node);
static inline void _program_item(ast_program_item_t* node);
static inline void _start_block(ast_start_block_t* node);
static inline void _type_name(ast_type_name_t* node);
static inline void _formatted_string(ast_formatted_string_t* node);
static inline void _data_declaration(ast_data_declaration_t* node);
static inline void _data_declaration_list(ast_data_declaration_list_t* node);
static inline void _data_definition(ast_data_definition_t* node);
static inline void _func_definition(ast_func_definition_t* node);
static inline void _func_params(ast_func_params_t* node);
static inline void _func_body(ast_func_body_t* node);
static inline void _func_body_list(ast_func_body_list_t* node);
static inline void _loop_body(ast_loop_body_t* node);
static inline void _loop_body_list(ast_loop_body_list_t* node);
static inline void _loop_body_diffs(ast_loop_body_diffs_t* node);
static inline void _loop_body_elem(ast_loop_body_elem_t* node);
static inline void _func_body_elem(ast_func_body_elem_t* node);
static inline void _inline_statement(ast_inline_statement_t* node);
static inline void _return_statement(ast_return_statement_t* node);
static inline void _if_clause(ast_if_clause_t* node);
static inline void _ifelse_statement(ast_ifelse_statement_t* node);
static inline void _else_segment(ast_else_segment_t* node);
static inline void _final_else_segment(ast_final_else_segment_t* node);
static inline void _else_clause_list(ast_else_clause_list_t* node);
static inline void _else_clause(ast_else_clause_t* node);
static inline void _while_clause(ast_while_clause_t* node);
static inline void _while_statement(ast_while_statement_t* node);
static inline void _do_statement(ast_do_statement_t* node);
static inline void _assignment_right(ast_assignment_right_t* node);
static inline void _assignment(ast_assignment_t* node);
static inline void _raw_value(ast_raw_value_t* node);
static inline void _expr_primary(ast_expr_primary_t* node);
static inline void _func_reference(ast_func_reference_t* node);
static inline void _expression(ast_expression_t* node);
static inline void _expression_list(ast_expression_list_t* node);
static inline void _expression_list_param(ast_expression_list_param_t* node);
static inline void _expression_param(ast_expression_param_t* node);

/*
 * program
 *    : program_item_list
 *    ;
 */
static inline void _program(ast_program_t* node) {

    ENTER;

    _program_item_list(node->program_item_list);

    RETURN();
}

/*
 * program_item_list
 *    : program_item
 *    | program_item_list program_item
 *    ;
 */
static inline void _program_item_list(ast_program_item_list_t* node) {

    ENTER;

    int mark = 0;
    ast_program_item_t* ptr;

    node->context = create_context();
    while(NULL != (ptr = iterate_pointer_list(node->list, &mark))) {
        _program_item(ptr);
    }
    pop_context();

    RETURN();
}

/*
 * import_statement
 *    : IMPORT STRING_LIT
 *    ;
 */
static inline void _import_statement(ast_import_statement_t* node) {

    ENTER;

    RETURN();
}

/*
 * program_item
 *    : data_definition
 *    | func_definition
 *    | func_body
 *    | import_statement
 *    | exception_identifier
 *    ;
 */
static inline void _program_item(ast_program_item_t* node) {

    ENTER;

    switch(node->nterm->type) {
        case AST_DATA_DECLARATION:
            _data_declaration((ast_data_declaration_t*)node->nterm);
            break;
        case AST_FUNC_DEFINITION:
            _func_definition((ast_func_definition_t*)node->nterm);
            break;
        case AST_FUNC_BODY:
            _func_body((ast_func_body_t*)node->nterm);
            break;
        case AST_IMPORT_STATEMENT:
            _import_statement((ast_import_statement_t*)node->nterm);
            break;
        case AST_START_BLOCK:
            _start_block((ast_start_block_t*)node->nterm);
            break;
        default:
            FATAL("unknown node type: %s (%d)", node_type_to_name(node->nterm), node->nterm->type);
    }

    RETURN();
}

/*
 * start_block
 *    : START func_body
 *    ;
 */
static inline void _start_block(ast_start_block_t* node) {

    ENTER;

    _func_body(node->func_body);

    RETURN();
}

/*
 * type_name
 *    : INTEGER
 *    | FLOAT
 *    | STRING
 *    | LIST
 *    | HASH
 *    ;
 */
static inline void _type_name(ast_type_name_t* node) {

    ENTER;

    RETURN();
}

/*
 * formatted_string
 *    : STRING_LIT expression_list_param
 *    | STRING_LIT
 *    ;
 */
static inline void _formatted_string(ast_formatted_string_t* node) {

    ENTER;

    _expression_list_param(node->expression_list_param);

    RETURN();
}

/*
 * Generate a data definition symbol.
 *
 * data_declaration
 *    : type_name IDENTIFIER
 *    | CONST type_name IDENTIFIER
 *    ;
 */
static inline void _data_declaration(ast_data_declaration_t* node) {

    ENTER;

    symbol_t* sym = create_symbol(SYM_DATA);
    sym->name = node->IDENTIFIER->raw;
    sym->node = (ast_node_t*)node;
    sym->tok = node->IDENTIFIER;
    sym->is_const = node->is_const;
    sym->is_iter = false;
    sym->ref_count = 0;
    sym->sym_type = node->type_name->token->type;

    add_symbol(peek_context(), sym->name, sym);

    //_type_name(node->type_name);

    RETURN();
}

/*
 * data_declaration_list
 *    : data_declaration
 *    | data_declaration_list ',' data_declaration
 *    ;
 */
static inline void _data_declaration_list(ast_data_declaration_list_t* node) {

    ENTER;

    int mark = 0;
    ast_data_declaration_t* ptr;

    while(NULL != (ptr = iterate_pointer_list(node->list, &mark))) {
        _data_declaration(ptr);
    }

    RETURN();
}

/*
 * data_definition
 *    : data_declaration
 *    | data_declaration '=' expression
 *    | data_declaration '=' list_init
 *    | data_declaration '=' dict_init
 *    ;
 */
static inline void _data_definition(ast_data_definition_t* node) {

    ENTER;

    _data_declaration(node->data_declaration);

    if(node->expression != NULL)
        _expression(node->expression);

    RETURN();
}

/*
 * Add the parameters in their own symbol context and finish generating
 * the decorated function name.
 *
 * func_definition
 *    : func_name func_params func_body
 *    ;
 */
static inline void _func_definition(ast_func_definition_t* node) {

    ENTER;

    // generate the function prototype for error checking.
    // this has to be done first in the case of recursion.
    node->proto = create_string_buffer(token_to_str(node->func_name->type_name->token->type));
    append_string_buffer_fmt(node->proto, " %s(", node->func_name->IDENTIFIER->raw);

    pointer_list_t* lst = node->func_params->data_declaration_list->list;
    ast_data_declaration_t* dd;
    int mark = 0;
    while(NULL != (dd = iterate_pointer_list(lst, &mark))) {
        node->arity++;
        append_string_buffer_fmt(node->proto, "%s", token_to_str(dd->type_name->token->type));
        if(mark < lst->len)
            append_string_buffer_char(node->proto, ',');
    }
    append_string_buffer_char(node->proto, ')');

    // create and store the symbol
    symbol_t* sym = create_symbol(SYM_FUNC);
    sym->name = node->func_name->IDENTIFIER->raw;
    sym->node = (ast_node_t*)node;
    sym->tok = node->func_name->IDENTIFIER;
    sym->is_const = true;
    sym->ref_count = 0;
    sym->sym_type = node->func_name->type_name->token->type;

    add_symbol(peek_context(), sym->name, sym);

    // no need to traverse because it's practically done here.
    //_func_name(node->func_name);

    // create the context and store the func parms and the func_body
    node->context = create_context();

    _func_params(node->func_params);
    _func_body(node->func_body);

    pop_context();

    TRACE("definition proto: %s (%d)", raw_string_buffer(node->proto), node->arity);

    RETURN();
}

/*
 * func_params
 *    : '(' data_declaration_list ')'
 *    | '(' ')'
 *    ;
 */
static inline void _func_params(ast_func_params_t* node) {

    ENTER;

    _data_declaration_list(node->data_declaration_list);

    RETURN();
}

/*
 * func_body
 *    : '{' func_body_list '}'
 *    ;
 */
static inline void _func_body(ast_func_body_t* node) {

    ENTER;

    _func_body_list(node->func_body_list);

    RETURN();
}

/*
 * func_body_list
 *    : func_body_elem
 *    | func_body_list func_body_elem
 *    ;
 */
static inline void _func_body_list(ast_func_body_list_t* node) {

    ENTER;

    int mark = 0;
    ast_func_body_elem_t* ptr;

    node->context = create_context();
    while(NULL != (ptr = iterate_pointer_list(node->list, &mark))) {
        _func_body_elem(ptr);
    }
    pop_context();

    RETURN();
}

/*
 * loop_body
 *    : '{' '}'
 *    | '{' loop_body_list '}'
 *    ;
 */
static inline void _loop_body(ast_loop_body_t* node) {

    ENTER;

    _loop_body_list(node->loop_body_list);

    RETURN();
}

/*
 * loop_body_list
 *    : loop_body_elem
 *    | loop_body_list loop_body_elem
 *    ;
 */
static inline void _loop_body_list(ast_loop_body_list_t* node) {

    ENTER;

    int mark = 0;
    ast_loop_body_elem_t* ptr;

    node->context = create_context();
    while(NULL != (ptr = iterate_pointer_list(node->list, &mark))) {
        _loop_body_elem(ptr);
    }
    pop_context();

    RETURN();
}

/*
 * loop_body_diffs
 *    : BREAK
 *    | CONTINUE
 *    ;
 */
static inline void _loop_body_diffs(ast_loop_body_diffs_t* node) {

    ENTER;

    RETURN();
}

/*
 * loop_body_elem
 *    : func_body_elem
 *    | loop_body_diffs
 *    ;
 */
static inline void _loop_body_elem(ast_loop_body_elem_t* node) {

    ENTER;

    switch(node->nterm->type) {
        case AST_LOOP_BODY_DIFFS:
            _loop_body_diffs((ast_loop_body_diffs_t*)node->nterm);
            break;
        case AST_FUNC_BODY_ELEM:
            _func_body_elem((ast_func_body_elem_t*)node->nterm);
            break;
        default:
            FATAL("unknown node type: %s (%d)", node_type_to_name(node->nterm), node->nterm->type);
    }

    RETURN();
}

/*
 * func_body_elem
 *    : data_definition
 *    | func_reference
 *    | assignment
 *    | while_statement
 *    | do_statement
 *    | for_statement
 *    | ifelse_statement
 *    | return_statement
 *    | exit_statement
 *    | print_statement
 *    | trace_statement
 *    | func_body
 *    ;
 */
static inline void _func_body_elem(ast_func_body_elem_t* node) {

    ENTER;

    switch(node->nterm->type) {
        case AST_DATA_DEFINITION:
            _data_definition((ast_data_definition_t*)node->nterm);
            break;
        case AST_FUNC_REFERENCE:
            _func_reference((ast_func_reference_t*)node->nterm);
            break;
        case AST_ASSIGNMENT:
            _assignment((ast_assignment_t*)node->nterm);
            break;
        case AST_WHILE_STATEMENT:
            _while_statement((ast_while_statement_t*)node->nterm);
            break;
        case AST_DO_STATEMENT:
            _do_statement((ast_do_statement_t*)node->nterm);
            break;
        case AST_IFELSE_STATEMENT:
            _ifelse_statement((ast_ifelse_statement_t*)node->nterm);
            break;
        case AST_RETURN_STATEMENT:
            _return_statement((ast_return_statement_t*)node->nterm);
            break;
        case AST_INLINE_STATEMENT:
            _inline_statement((ast_inline_statement_t*)node->nterm);
            break;
        case AST_FUNC_BODY:
            _func_body((ast_func_body_t*)node->nterm);
            break;
        default:
            FATAL("unknown node type: %s (%d)", node_type_to_name(node->nterm), node->nterm->type);
    }

    RETURN();
}

/*
 * inline_statement
 *    : INLINE
 *    ;
 */
static inline void _inline_statement(ast_inline_statement_t* node) {

    ENTER;

    RETURN();
}

/*
 * return_statement
 *    : RETURN expression_param
 *    | RETURN
 *    ;
 */
static inline void _return_statement(ast_return_statement_t* node) {

    ENTER;

    _expression_param(node->expression_param);

    RETURN();
}


/*
 * if_clause
 *    : IF '(' expression ')' func_body
 *    ;
 */
static inline void _if_clause(ast_if_clause_t* node) {

    ENTER;

    _expression(node->expression);
    _func_body(node->func_body);

    RETURN();
}

/*
 * ifelse_statement
 *    : if_clause
 *    | if_clause else_clause
 *    ;
 */
static inline void _ifelse_statement(ast_ifelse_statement_t* node) {

    ENTER;

    _if_clause(node->if_clause);

    if(node->else_clause != NULL)
        _else_clause(node->else_clause);

    RETURN();
}

/*
 * else_segment
 *    : ELSE '(' expression ')' func_body
 *    ;
 */
static inline void _else_segment(ast_else_segment_t* node) {

    ENTER;

    _expression(node->expression);
    _func_body(node->func_body);

    RETURN();
}

/*
 * final_else_segment
 *    : ELSE '(' ')' func_body
 *    | ELSE func_body
 *    ;
 */
static inline void _final_else_segment(ast_final_else_segment_t* node) {

    ENTER;

    _func_body(node->func_body);

    RETURN();
}

/*
 * else_clause_list
 *    : else_segment
 *    | else_clause_list else_segment
 *    ;
 */
static inline void _else_clause_list(ast_else_clause_list_t* node) {

    ENTER;

    int mark = 0;
    ast_else_segment_t* ptr;

    while(NULL != (ptr = iterate_pointer_list(node->list, &mark))) {
        _else_segment(ptr);
    }

    RETURN();
}

/*
 * else_clause
 *    : else_clause_list
 *    | else_clause_list final_else_segment
 *    | final_else_segment
 *    ;
 */
static inline void _else_clause(ast_else_clause_t* node) {

    ENTER;

    if(node->else_clause_list != NULL)
        _else_clause_list(node->else_clause_list);

    if(node->final_else_segment != NULL)
        _final_else_segment(node->final_else_segment);

    RETURN();
}

/*
 * while_clause
 *    : WHILE expression_param
 *    | WHILE
 *    ;
 */
static inline void _while_clause(ast_while_clause_t* node) {

    ENTER;

    _expression_param(node->expression_param);

    RETURN();
}

/*
 * while_statement
 *    : while_clause loop_body
 *    ;
 */
static inline void _while_statement(ast_while_statement_t* node) {

    ENTER;

    _while_clause(node->while_clause);
    _loop_body(node->loop_body);

    RETURN();
}

/*
 * do_statement
 *    : DO loop_body while_clause
 *    ;
 */
static inline void _do_statement(ast_do_statement_t* node) {

    ENTER;

    _loop_body(node->loop_body);
    _while_clause(node->while_clause);

    RETURN();
}

/*
 * assignment_right
 *    : expression
 *    | '(' type_name ')' expression
 *    ;
 */
static inline void _assignment_right(ast_assignment_right_t* node) {

    ENTER;

    if(node->type_name != NULL)
        _type_name(node->type_name);
    _expression(node->expression);

    RETURN();
}


/*
 * assignment
 *    : assignment_left '=' assignment_right
 *    ;
 */
static inline void _assignment(ast_assignment_t* node) {

    ENTER;


    _assignment_right(node->assignment_right);

    symbol_t* sym = find_symbol(peek_context(), node->IDENTIFIER->raw);
    if(sym != NULL) {
        if(sym->sym_class != SYM_DATA)
            syntax_error(node->IDENTIFIER, "cannot assign to a function name: \"%s\"", sym->name);
        else {
            check_assignment_type(node->IDENTIFIER, sym->sym_type, node->assignment_right->type);
            sym->ref_count++;
        }
    }
    else
        syntax_error(node->IDENTIFIER, "symbol \"%s\" not found", node->IDENTIFIER->raw);


    RETURN();
}

/*
 * references only.
 *
 * raw_value
 *    : IDENTIFIER
 *    | INTEGER_LIT
 *    | FLOAT_LIT
 *    ;
 */
static inline void _raw_value(ast_raw_value_t* node) {

    ENTER;

    if(node->token->type == IDENTIFIER) {
        // validate the reference
        symbol_t* sym = find_symbol(peek_context(), node->token->raw);
        if(sym != NULL) {
            node->type = sym->sym_type;
            sym->ref_count++;
        }
        else
            syntax_error(node->token, "symbol \"%s\" not defined", node->token->raw);
    }

    TRACE("type set to: %s", token_to_str(node->type));

    RETURN();
}

/*
 * expr_primary
 *    : raw_value
 *    | formatted_string
 *    ;
 */
static inline void _expr_primary(ast_expr_primary_t* node) {

    ENTER;

    switch(node->nterm->type) {
        case AST_RAW_VALUE:
            _raw_value((ast_raw_value_t*)node->nterm);
            node->type = ((ast_raw_value_t*)node->nterm)->type;
            break;
        case AST_FORMATTED_STRING:
            _formatted_string((ast_formatted_string_t*)node->nterm);
            node->type = STRING;
            break;
        case AST_FUNC_REFERENCE:
            _func_reference((ast_func_reference_t*)node->nterm);
            node->type = ((ast_func_reference_t*)node->nterm)->type;
            break;
        default:
            FATAL("unknown node type: %s (%d)", node_type_to_name(node->nterm), node->nterm->type);
    }

    RETURN();
}

/*
 * func_reference
 *    : IDENTIFIER expression_list_param
 *    ;
 */
static inline void _func_reference(ast_func_reference_t* node) {

    ENTER;

    // validate the function reference
    symbol_t* sym = find_symbol(peek_context(), node->IDENTIFIER->raw);
    if(sym != NULL) {
        if(sym->sym_class != SYM_FUNC)
            syntax_error(node->IDENTIFIER, "symbol \"%s\" is not a function", node->IDENTIFIER->raw);
        else {
            // check the parameters for existence and set the types for exprs
            _expression_list_param(node->expression_list_param);

            // generate the proto to check against the definition.
            node->proto = create_string_buffer(token_to_str(sym->sym_type));
            append_string_buffer_fmt(node->proto, " %s(", node->IDENTIFIER->raw);

            pointer_list_t* lst = node->expression_list_param->expression_list->list;
            ast_expression_t* ptr;
            int mark = 0;

            while(NULL != (ptr = iterate_pointer_list(lst, &mark))) {
                node->arity++;
                append_string_buffer_str(node->proto, token_to_str(ptr->type));
                if(mark < lst->len)
                    append_string_buffer_char(node->proto, ',');
            }
            append_string_buffer_char(node->proto, ')');

            node->type = sym->sym_type;
            sym->ref_count++;

            TRACE("reference proto: %s", raw_string_buffer(node->proto));

            ast_func_definition_t* sym_node = ((ast_func_definition_t*)sym->node);
            if(node->arity != sym_node->arity)
                syntax_error(node->IDENTIFIER, "expected %d function parameters but got %d",
                                sym_node->arity, node->arity);
            else if(cmp_string_buffer(sym_node->proto, node->proto)) {
                syntax_error(node->IDENTIFIER, "expected function proto %s but got %s",
                    sym_node->proto, node->proto);
            }
        }
    }
    else
        syntax_error(node->IDENTIFIER, "function \"%s\" not defined", node->IDENTIFIER->raw);

    RETURN();
}

/*
 * expression
 *    : expr_primary
 *    | expression ADD_OPER expression
 *    | expression SUB_OPER expression
 *    | expression MUL_OPER expression
 *    | expression DIV_OPER expression
 *    | expression MOD_OPER expression
 *    | expression POW_OPER expression
 *    | expression GT_OPER expression
 *    | expression LT_OPER expression
 *    | expression EQU_OPER expression
 *    | expression NEQ_OPER expression
 *    | expression LTE_OPER expression
 *    | expression GTE_OPER expression
 *    | expression OR_OPER expression
 *    | expression AND_OPER expression
 *    | NOT_OPER expression %prec UNARY
 *    | SUB_OPER expression %prec UNARY
 *    | '(' expression ')'
 *    ;
 */
static inline void _expression(ast_expression_t* node) {

    ENTER;

    if(node->expr_primary != NULL) {
        _expr_primary(node->expr_primary);
        node->type = node->expr_primary->type;
        TRACE("HERE: %s", token_to_str(node->type));
    }
    else if(node->expression != NULL) {
        _expression(node->expression);
        node->type = node->expression->type;
    }
    else if(node->oper->type == NOT_OPER) {
        _expression(node->right);
        node->type = check_unary_expression_type(node->oper, node->right->type);
    }
    else if(node->oper->type == UNARY_MINUS_OPER) {
        _expression(node->right);
        node->type = check_unary_expression_type(node->oper, node->right->type);
    }
    else {
        _expression(node->left);
        _expression(node->right);
        node->type = check_binary_expression_type(node->oper, node->left->type, node->right->type);
    }

    RETURN();
}

/*
 * expression_list
 *    : expression
 *    | expression_list ',' expression
 *    ;
 */
static inline void _expression_list(ast_expression_list_t* node) {

    ENTER;

    int mark = 0;
    ast_expression_t* ptr;

    while(NULL != (ptr = iterate_pointer_list(node->list, &mark))) {
        _expression(ptr);
    }

    RETURN();
}

/*
 * expression_list_param
 *    : '(' expression_list ')'
 *    | '(' ')'
 *    ;
 */
static inline void _expression_list_param(ast_expression_list_param_t* node) {

    ENTER;

    _expression_list(node->expression_list);

    RETURN();
}

/*
 * expression_param
 *    : '(' expression ')'
 *    | '(' ')'
 *    ;
 */
static inline void _expression_param(ast_expression_param_t* node) {

    ENTER;

    _expression(node->expression);

    RETURN();
}


void create_symbol_table(void) {

    ENTER;

    if(get_errors() == 0) {
        _program(get_program());
    }

    RETURN();
}