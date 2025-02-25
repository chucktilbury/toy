
/*
 *  This AST pass creates the symbol table. The symbol context has already been
 *  created byt the parser.
 */
#include <stdio.h>
#include <string.h>

#include "symbol_table.h"
#include "context.h"
#include "errors.h"
#include "string_buffer.h"
#include "symbol.h"
#include "ast.h"
#include "parser.h"

static int start_block_count = 0;

#define SYMTAB_TRACE

#ifdef SYMTAB_TRACE
#define USE_TRACE
#endif
#include "trace.h"

#ifdef SYMTAB_TRACE
#define SYMTAB_ENTER(t)                                                                                            \
    do {                                                                                                           \
        ENTER;                                                                                                     \
        ASSERT(node != NULL, "expected a %s node but got NULL", #t);                                               \
        ASSERT(node->node.type == t, "expected node type %s but got type %s", #t, node_type_to_str((ast_node_t*)node)); \
    } while(0)

#define SYMTAB_RETURN \
    do {              \
        RETURN();     \
    } while(0)

#define TRACE_CONTEXT(c)                                                              \
    do {                                                                              \
        TRACE("context: %p: %d: %d", (void*)(c), (c)->table->cap, (c)->table->count); \
    } while(0)

#else
#define SYMTAB_ENTER(t)
#define SYMTAB_RETURN
#define TRACE_CONTEXT(c)
#endif

static inline void create_data_symbol(ast_data_definition_t* node) {

    SYMTAB_ENTER(AST_DATA_DEFINITION);

    symbol_t* sym = create_symbol(SYM_DATA);
    sym->is_iter = false;
    sym->is_init = node->is_init;
    sym->node = (ast_node_t*)node;

    ast_data_declaration_t* dd = node->data_declaration;
    sym->is_const = dd->is_const;
    sym->sym_type = dd->type_name->token->type;
    sym->tok = dd->IDENTIFIER;
    sym->name = dd->IDENTIFIER->raw;
    sym->ref_count = 0;

    add_symbol(peek_context(), sym->name, sym);

    TRACE("adding data symbol: %s", sym->name);
    SYMTAB_RETURN;
}

static inline void create_func_params(ast_func_definition_t* node) {

    SYMTAB_ENTER(AST_FUNC_DEFINITION);

    ast_func_params_t* parms = node->func_params;
    pointer_list_t* list = parms->data_declaration_list->list;
    parms->data_declaration_list->context = create_context();

    int mark = 0;
    ast_data_declaration_t* dd;
    while(NULL != (dd = iterate_pointer_list(list, &mark))) {
        symbol_t* sym = create_symbol(SYM_DATA);
        sym->is_iter = false;
        // assignment is not legal
        sym->is_init = true;
        sym->is_const = true;
        sym->node = (ast_node_t*)dd;

        sym->sym_type = dd->type_name->token->type;
        sym->tok = dd->IDENTIFIER;
        sym->name = dd->IDENTIFIER->raw;
        sym->ref_count = 0;
        node->arity++;

        TRACE("adding func param symbol: %s %s", token_to_str(sym->sym_type), sym->name);
        add_symbol(peek_context(), sym->name, sym);

        append_string_buffer_fmt(node->proto, "%s, ",
                                 token_to_str(dd->type_name->token->type));
    }

    if(node->arity > 0)
        trunc_string_buffer(node->proto, -2);
    append_string_buffer_char(node->proto, ')');

    SYMTAB_RETURN;
}

static inline void create_func_symbol(ast_func_definition_t* node) {

    SYMTAB_ENTER(AST_FUNC_DEFINITION);

    symbol_t* sym = create_symbol(SYM_FUNC);
    // cannot assign to it
    sym->is_const = true;
    sym->is_init = true;
    sym->node = (ast_node_t*)node;

    sym->is_iter = node->func_name->is_iterator;
    sym->sym_type = node->func_name->type_name->token->type;
    sym->name = node->func_name->IDENTIFIER->raw;
    sym->tok = node->func_name->IDENTIFIER;
    sym->ref_count = 0;

    add_symbol(peek_context(), sym->name, sym);
    TRACE("adding func symbol: %s", sym->name);

    node->proto = create_string_buffer(token_to_str(node->func_name->type_name->token->type));
    append_string_buffer_fmt(node->proto, " %s(", node->func_name->IDENTIFIER->raw);

    create_func_params(node);

    TRACE("definition proto: %s %s:%d:%d", raw_string_buffer(node->proto),
          sym->tok->fname, sym->tok->line_no, sym->tok->col_no);

    SYMTAB_RETURN;
}

static void pre(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_FUNC_BODY_LIST:
            ((ast_func_body_list_t*)node)->context = create_context();
            break;

        case AST_LOOP_BODY_LIST:
            ((ast_loop_body_list_t*)node)->context = create_context();
            break;

        case AST_PROGRAM_ITEM_LIST:
            ((ast_program_item_list_t*)node)->context = create_context();
            break;

        case AST_DATA_DEFINITION:
            create_data_symbol((ast_data_definition_t*)node);
            break;

        case AST_FUNC_DEFINITION:
            create_func_symbol((ast_func_definition_t*)node);
            break;

        case AST_START_BLOCK:
            start_block_count++;
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
            pop_context();
            break;

        default:
            break;
    }
}

void create_symbol_table(void) {

    SEPARATOR;
    ENTER;

    if(!get_errors()) {
        traverse_ast(pre, post);
    }

    if(start_block_count != 1) {
        if(start_block_count == 0)
            syntax_warning(NULL, "no start block found");
        else if(start_block_count > 1)
            syntax_error(NULL, "only one start block is allowed in the whole program space");
    }

    RETURN();
}
