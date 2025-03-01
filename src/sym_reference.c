/*
 * This module traverses the AST and verifies that all symbols referenced are
 * present and that they have acceptable type references. If there is a problem
 * then publish a syntax error. When the emit passes are called, then the
 * existance and use of variables has already been verified.
 */
#include <stdio.h>

#include "sym_reference.h"
#include "ast.h"
#include "errors.h"
#include "context.h"
#include "symbol.h"
#include "tokens.h"
#include "parser.h"
#include "type_table.h"

//#define TRACE_SYM_REF

#ifdef TRACE_SYM_REF
#define USE_TRACE
#endif
#include "trace.h"

#ifdef TRACE_SYM_REF

#endif

const char* token_to_str(int);
static context_t* ctx = NULL;

static inline void validate_assignment_reference(ast_assignment_t* node) {

    ENTER;

    int rtype = node->assignment_right->type;

    token_t* tok = node->IDENTIFIER;
    TRACE("identifier = %s %d %d", tok->raw, tok->line_no, tok->col_no);
    TRACE("ctx = %p", (void*)ctx);
    symbol_t* sym = find_symbol(ctx, tok->raw);
    if(sym != NULL) {
        node->type = check_assignment_type(tok, sym->sym_type, rtype);
        sym->ref_count++;
        TRACE("symbol \"%s\" of type %s:%s is found", tok->raw,
              (sym->sym_class) ? "DATA" : "FUNC", token_to_str(sym->sym_type));
    }
    else
        syntax_error(tok, "in assignment to \"%s\", symbol not defined", tok->raw);

    RETURN();
}

static inline void validate_func_reference(ast_func_reference_t* node) {

    ENTER;

    token_t* tok = node->IDENTIFIER;
    TRACE("ctx = %p", (void*)ctx);
    symbol_t* sym = find_symbol(ctx, tok->raw);
    if(sym != NULL) {
        if(sym->sym_class != SYM_FUNC)
            syntax_error(tok, "symbol \"%s\" is not a function", tok->raw);
        else {
            node->type = sym->sym_type;
            sym->ref_count++;

            TRACE("symbol \"%s\" of type %s:%s is found", tok->raw,
                  (sym->sym_class) ? "DATA" : "FUNC", token_to_str(sym->sym_type));
        }
    }
    else
        syntax_error(tok, "function definition for \"%s\" not found", tok->raw);

    RETURN();
}

static inline void set_expression_type(ast_expression_t* node) {

    ENTER;

    if(node->expr_primary != NULL) {
        node->type = node->expr_primary->type;
        TRACE("primary expr type = %s", token_to_str(node->type));
    }
    else if(node->expression != NULL) {
        node->type = node->expression->type;
        TRACE("(expression) type = %s", token_to_str(node->type));
    }
    else if(node->oper->type == NOT_OPER) {
        node->type = node->right->type;
        TRACE("not operator to type = %s", token_to_str(node->type));
    }
    else if(node->oper->type == UNARY_MINUS_OPER) {
        node->type = node->right->type;
        TRACE("negation to type = %s", token_to_str(node->type));
    }
    else {
        node->type = check_expression_type(node->oper,
                                           node->left->type,
                                           node->right->type);
        TRACE("%s", token_to_str(node->type));
    }

    RETURN();
}

static inline void validate_assignment_right(ast_assignment_right_t* node) {

    ENTER;

    if(node->type_name != NULL)
        node->type = check_expression_cast(node->type_name, node->expression);
    else
        node->type = node->expression->type;

    RETURN();
}

static inline void set_expression_primary(ast_expr_primary_t* node) {

    ENTER;

    int type = node->nterm->type;

    if(type == AST_RAW_VALUE) {
        node->type = ((ast_raw_value_t*)node->nterm)->type;
        TRACE("raw value: %s", token_to_str(node->type));
    }
    else if(type == AST_FUNC_REFERENCE) {
        node->type = ((ast_func_reference_t*)node->nterm)->type;
        TRACE("func reference: %s", token_to_str(node->type));
    }
    else if(type == AST_FORMATTED_STRING) {
        node->type = STRING;
        TRACE("formatted string: %s", token_to_str(node->type));
    }
    else
        FATAL("unknown primary node type: %s", node_type_to_str(node->nterm));

    RETURN();
}

static inline void set_raw_value(ast_raw_value_t* node) {

    ENTER;

    if(node->token->type == IDENTIFIER) {
        TRACE("ctx = %p", (void*)ctx);
        symbol_t* sym = find_symbol(ctx, node->token->raw);
        if(sym != NULL) {
            node->type = sym->sym_type;
            sym->ref_count++;
            TRACE("symbol \"%s\" of type %s:%s is found", node->token->raw,
                  (sym->sym_class) ? "DATA" : "FUNC", token_to_str(sym->sym_type));
        }
        else
            syntax_error(node->token, "symbol \"%s\" is not defined", node->token->raw);
    }
    // else set in parser

    RETURN();
}

static inline void validate_data_definition(ast_data_definition_t* node) {

    ENTER;

    if(node->expression != NULL) {
        node->type = check_assignment_type(node->data_declaration->IDENTIFIER,
                                           node->data_declaration->type,
                                           node->expression->type);
    }

    RETURN();
}

static inline void set_data_declaration(ast_data_declaration_t* node) {

    ENTER;

    node->type = node->type_name->token->type;

    RETURN();
}

static void pre(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_FUNC_BODY_LIST:
            ctx = ((ast_func_body_list_t*)node)->context;
            TRACE("AST_FUNC_BODY_LIST: %p", (void*)ctx);
            break;

        case AST_LOOP_BODY_LIST:
            ctx = ((ast_loop_body_list_t*)node)->context;
            TRACE("AST_LOOP_BODY_LIST: %p", (void*)ctx);
            break;

        case AST_DATA_DECLARATION_LIST:
            ctx = ((ast_data_declaration_list_t*)node)->context;
            TRACE("AST_DATA_DECLARATION_LIST: %p", (void*)ctx);
            break;

        case AST_PROGRAM_ITEM_LIST:
            ctx = ((ast_program_item_list_t*)node)->context;
            TRACE("AST_PROGRAM_ITEM_LIST: %p", (void*)ctx);
            break;

        default:
            break;
    }
}

static void post(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_FUNC_DEFINITION:
        case AST_FUNC_BODY_LIST:
        case AST_LOOP_BODY_LIST:
        case AST_PROGRAM_ITEM_LIST:
            ctx = ctx->prev;
            break;

        case AST_EXPRESSION:
            TRACE(">>> AST_EXPRESSION");
            set_expression_type((ast_expression_t*)node);
            TRACE("<<< AST_EXPRESSION");
            break;

        case AST_EXPR_PRIMARY:
            TRACE("AST_EXPR_PRIMARY");
            set_expression_primary((ast_expr_primary_t*)node);
            break;

        case AST_RAW_VALUE:
            TRACE("AST_RAW_VALUE");
            set_raw_value((ast_raw_value_t*)node);
            break;

        case AST_DATA_DECLARATION:
            TRACE("AST_DATA_DECLARATION");
            set_data_declaration((ast_data_declaration_t*)node);
            break;

        case AST_ASSIGNMENT:
            TRACE("AST_ASSIGNMENT");
            validate_assignment_reference((ast_assignment_t*)node);
            break;

        case AST_FUNC_REFERENCE:
            TRACE("AST_FUNC_REFERENCE");
            validate_func_reference((ast_func_reference_t*)node);
            break;

        case AST_ASSIGNMENT_RIGHT:
            TRACE("AST_ASSIGNMENT_RIGHT");
            validate_assignment_right((ast_assignment_right_t*)node);
            break;

        case AST_DATA_DEFINITION:
            TRACE("AST_DATA_DEFINITION");
            validate_data_definition((ast_data_definition_t*)node);
            break;

        default:
            break;
    }
}

void check_sym_refs(void) {

    SEPARATOR;
    ENTER;

    if(!get_errors()) {
        ctx = reset_context();
        traverse_ast(pre, post);
    }

    RETURN();
}
