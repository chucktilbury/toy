#ifndef _CONTEXT_
#define _CONTEXT_

#include "common_hash.h"

typedef struct _context_t_ {
    hash_table_t* table;
    struct _context_t_* prev;
} context_t;

void init_context(void);
context_t* create_context(void);
void push_context(context_t*);
context_t* pop_context(void);
context_t* peek_context(void);

// void add_context_item(const char*, void*);
// void* seek_context_item(const char*);
context_t* reset_context(void);

#endif /* _CONTEXT_ */
