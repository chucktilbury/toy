#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stdint.h>
#include <stdbool.h>

#include "ast.h"
#include "hash.h"

typedef enum {
    SYM_FUNC,
    SYM_DATA,
} symbol_type_t;

typedef struct {
    symbol_type_t symbol_type;
    int data_type; // from the parser
    const char* name;
    bool is_init;
    bool is_const;
    ast_node_t* node;
    union {
        uint64_t integer;
        double real;
        const char* text;
        void* other;
    } val;
} symbol_t;

typedef struct _sym_context_t_ {
    struct _sym_context_t_* child;
    struct _sym_context_t_* parent;
    hash_table_t* table;
} sym_context_t;

void create_symtab(void);

void push_sym_context(sym_context_t*);
sym_context_t* pop_sym_context(void);
sym_context_t* peek_sym_context(void);

symbol_t* find_symbol(const char* key);

#endif /* _SYMTAB_H_ */
