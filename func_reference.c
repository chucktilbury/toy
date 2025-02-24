#include <stdio.h>

#include "ast.h"
#include "errors.h"
#include "context.h"
#include "pointer_list.h"
#include "string_buffer.h"
#include "symbol.h"


#define TRACE_FUNC_REF

#ifdef TRACE_FUNC_REF
#define USE_TRACE
#endif
#include "trace.h"

const char* token_to_str(int);
static context_t* ctx = NULL;


static void check_func_ref(ast_func_reference_t* node) {

    // existance of symbol is done in sym_reference.c
    symbol_t* sym = find_symbol(ctx, node->IDENTIFIER->raw);

    ast_func_definition_t* def = ((ast_func_definition_t*)sym->node);
    if(def->arity != node->arity)
        syntax_error(node->IDENTIFIER, "expected %d parameters but got %d: %s",
                     def->arity, node->arity, def->proto->buf);
    else if(cmp_string_buffer(def->proto, node->proto))
        syntax_error(node->IDENTIFIER, "function defined as %s but called as %s",
                     node->proto->buf, def->proto->buf);
}


static void make_func_ref_proto(ast_func_reference_t* node) {

    node->proto = create_string_buffer(NULL);
    append_string_buffer_fmt(node->proto, "%s ", token_to_str(node->type));
    append_string_buffer_fmt(node->proto, "%s(", node->IDENTIFIER->raw);

    ast_expression_t* expr;
    int mark = 0;
    pointer_list_t* list = node->expression_list_param->expression_list->list;

    expr = iterate_pointer_list(list, &mark);
    while(true) {
        if(expr != NULL) {
            append_string_buffer_str(node->proto, token_to_str(expr->type));
            node->arity++;
        }
        else
            break;

        expr = iterate_pointer_list(list, &mark);

        if(expr != NULL) {
            append_string_buffer_str(node->proto, ", ");
        }
    }
    append_string_buffer_char(node->proto, ')');

    token_t* tok = node->IDENTIFIER;
    TRACE("reference proto: %s %s:%d:%d", raw_string_buffer(node->proto),
          tok->fname, tok->line_no, tok->col_no);

    check_func_ref(node);
}

static void pre(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_FUNC_REFERENCE:
            make_func_ref_proto((ast_func_reference_t*)node);
            break;

        default:
            break;
    }
}

void create_func_references(void) {

    SEPARATOR;
    ENTER;

    if(!get_errors()) {
        // all functions are defined in the root context
        ctx = reset_context();
        traverse_ast(pre, NULL);
    }

    RETURN();
}


