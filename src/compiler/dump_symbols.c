/**
 * @file dump_symbols.c
 *
 * @brief This module traverses the AST and verifies that all symbols
 * referenced are present and that they have acceptable type references. If
 * there is a problem then publish a syntax error. When the emit passes are
 * called, then the existance and use of variables has already been verified.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-02-28
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include <stdio.h>

#include "ast.h"
#include "syntax_errors.h"
#include "errors.h"
#include "context.h"
#include "symbol.h"


#define TRACE_DUMP_SYM

#ifdef TRACE_DUMP_SYM
#define USE_TRACE
#endif
#include "trace.h"

#ifdef TRACE_DUMP_SYM

const char* token_to_str(int);

#define DUMP dump_context(ctx)
/**
 * @brief Dump the context. Only if tracing in this module is enabled.
 *
 * @param ctx
 */
static void dump_context(context_t* ctx) {

    ASSERT(ctx != NULL, "invalid context");

    if(ctx->table->count == 0)
        return;

    TRACE("this ptr: %p", (void*)ctx);
    TRACE("prev ptr: %p", (void*)ctx->prev);

    hash_table_t* tab = ctx->table;
    TRACE("table cap: %d", tab->cap);
    TRACE("table count: %d", tab->count);

    symbol_t* sym;
    for(int i = 0; i < tab->cap; i++) {
        if(tab->table[i] != NULL) {
            if(tab->table[i]->key != NULL) {
                sym = tab->table[i]->data;
                TRACE("symbol: %s %s %s -- %s:%s:%d",
                      sym->sym_class ? "DATA" : "FUNC",
                      token_to_str(sym->sym_type), sym->name,
                      sym->is_iter ? "true" : "false",
                      sym->is_const ? "true" : "false",
                      sym->ref_count);
            }
        }
    }
}

#else
#define DUMP
#endif

static context_t* ctx = NULL;

/**
 * @brief Set the context for a function body.
 *
 * @param node
 */
static inline void func_body(ast_func_body_list_t* node) {

    ENTER;
    ctx = ((ast_func_body_list_t*)node)->context;
    DUMP;
    RETURN();
}

/**
 * @brief Set the context for a loop body.
 *
 * @param node
 */
static inline void loop_body(ast_loop_body_list_t* node) {

    ENTER;
    ctx = ((ast_loop_body_list_t*)node)->context;
    DUMP;
    RETURN();
}

/**
 * @brief Set the context for func parameters.
 *
 * @param node
 */
static inline void func_parms(ast_func_definition_t* node) {

    ENTER;
    ctx = ((ast_func_definition_t*)node)->context;
    DUMP;
    RETURN();
}

/**
 * @brief Set the root context.
 *
 * @param node
 */
static inline void program_list(ast_program_item_list_t* node) {

    ENTER;
    ctx = ((ast_program_item_list_t*)node)->context;
    DUMP;
    RETURN();
}

/**
 * @brief Callback run before the node is traversed.
 *
 * @param node
 */
static void pre(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_FUNC_BODY_LIST:
            func_body((ast_func_body_list_t*)node);
            break;

        case AST_LOOP_BODY_LIST:
            loop_body((ast_loop_body_list_t*)node);
            break;

        //case AST_DATA_DECLARATION_LIST:
        case AST_FUNC_DEFINITION:
            //func_parms((ast_data_declaration_list_t*)node);
            func_parms((ast_func_definition_t*)node);
            break;

        case AST_PROGRAM_ITEM_LIST:
            program_list((ast_program_item_list_t*)node);
            break;

        default:
            break;
    }
}

/**
 * @brief External interface for the pass.
 *
 */
void dump_symbols(void) {

    SEPARATOR;
    ENTER;

    if(!get_errors()) {
        ctx = reset_context();
        traverse_ast(pre, NULL);
    }

    RETURN();
}
