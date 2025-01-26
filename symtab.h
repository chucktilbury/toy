#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stdint.h>

#include "hash.h"

typedef enum {
    SYM_INT,
    SYM_FLOAT,
    SYM_STR,
    SYM_LIST,
    SYM_HASH,
    SYM_FUNC,
} symbol_type_t;

typedef struct {
    symbol_type_t type;
} sym_node_t;

typedef struct {
    sym_node_t node;
    const char* name;
    union {
        uint64_t integer;
        double real;
        const char* text;
        void* other;
    } val;
    struct _sym_context_t_* child;
} symbol_t;

typedef struct _sym_context_t_ {
    struct _sym_context_t_* prev;
    hash_table_t* table;
} sym_context_t;

sym_context_t* create_sym_context(void);
void push_sym_context(sym_context_t*);
sym_context_t* pop_sym_context(void);
sym_context_t* peek_sym_context(void);

symbol_t* create_symbol(sym_context_t* cont, const char* key, symbol_type_t type);
symbol_t* find_symbol(const char* key);

#endif /* _SYMTAB_H_ */
