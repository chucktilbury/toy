#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stdint.h>
#include <stdbool.h>

#include "ast.h"
#include "hash.h"
#include "tokens.h"

typedef enum {
    SYM_FUNC,
    SYM_DATA,
    SYM_EXCEPT,
} symbol_type_t;

typedef struct {
    symbol_type_t symbol_type;
    token_t* tok;     // the token that holds the symbol from the source
    int data_type;    // type that the symbol represents from the parser
    const char* name; // the decorated hash table key
    bool is_init;     // true if the symbol is considered initialized
    bool is_const;    // true if the symbol represents a constant value
    bool is_iter;     // true if the function is an iterator
    int ref_count;    // reference count, used to detect errors
} symbol_t;

typedef struct _sym_context_t_ {
    struct _sym_context_t_* child;  // result of a push
    struct _sym_context_t_* parent; // result of a pop
    hash_table_t* table;            // table of the symbols in this context
} sym_context_t;

// Note that different passes implement their own push and pop functions in
// order to remain reentrant, according to their needs.

// call traverse_ast() to create the global symbol table.
sym_context_t* create_symtab(void);

// return a pointer to the global symbol table.
sym_context_t* get_symtab(void);

void dump_symbols(void);

#endif /* _SYMTAB_H_ */
