#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <stdbool.h>
#include "ast.h"
#include "tokens.h"
#include "context.h"

typedef enum {
    SYM_FUNC,
    SYM_DATA,
} symbol_class_t;

typedef struct {
    const char* name;         // decorated name for hash table
    symbol_class_t sym_class; // general handling of the symbol
    int sym_type;             // type user defined in source code
    token_t* tok;             // identifier that defined the name
    bool is_const;            // const keyword was found
    bool is_iter;             // functions only
    int ref_count;            // updated by the refernce checker
    ast_node_t* node;         // node that defines the symbol
} symbol_t;

symbol_t* create_symbol(symbol_class_t type);
void add_symbol(context_t* ctx, const char* name, symbol_t* node);
symbol_t* find_symbol(context_t* ctx, const char* name);

#endif /* _SYMBOL_H_ */
