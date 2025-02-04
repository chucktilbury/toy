#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "parser.h"
#include "pointer_list.h"

typedef enum {
    SYM_FUNC,
    SYM_DATA,
    SYM_EXCEPT,
} symbol_class_t;

typedef struct {
    const char* name;         // decorated name for hash table
    symbol_class_t sym_class; // general handling of the symbol
    int sym_type;             // type user defined in source code
    token_t* tok;             // token that defined the name
    bool is_const;            // const keyword wous found
    bool is_init;             // initializer was found in the syntax
    bool is_iter;             // functions only
    int ref_count;            // updated by the refernce checker
} symbol_t;

typedef struct _symbol_table_t_ {
    struct _symbol_table_t_* parent;
    hash_table_t* table;
} symbol_table_t;

/*
// global symbol table and context
extern symbol_table_t* symbol_table;
extern pointer_list_t* symbol_context;

symbol_t* create_symbol(void);
void add_symbol(symbol_t* sym);
void push_sym_context(void);
void* pop_sym_context(void);
symbol_table_t* peek_sym_context(void);
symbol_table_t* create_symtab(void);
*/

symbol_t* create_symbol(symbol_class_t, token_t*);

#endif /* _SYMTAB_H_ */
