
#include "memory.h"
#include "symbol.h"
#include "hash.h"
#include "syntax_errors.h"

const char* token_to_str(int);

symbol_t* create_symbol(symbol_class_t type) {

    symbol_t* ptr = _ALLOC_TYPE(symbol_t);

    ptr->sym_class = type;

    return ptr;
}

// actually stores objects of type ast_node_t*
void add_symbol(context_t* ctx, const char* name, symbol_t* node) {

    symbol_t* sym = find_symbol(ctx, name);
    if(NULL == sym)
        insert_hashtable(ctx->table, name, node);
    else
        syntax_error(node->tok, "symbol \"%s\" already defined as a %s at %s:%d:%d",
                     name, token_to_str(sym->sym_type),
                     sym->tok->fname, sym->tok->line_no, sym->tok->col_no);
}

symbol_t* find_symbol(context_t* ctx, const char* name) {

    void* data     = NULL;
    context_t* ptr = ctx;

    while(true) {
        if(find_hashtable(ptr->table, name, &data)) {
            break;
        }
        else if(ptr->prev != NULL) {
            ptr = ptr->prev;
        }
        else {
            data = NULL;
            break;
        }
    }

    return data;
}
