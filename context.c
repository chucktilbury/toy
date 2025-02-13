
#include <stdbool.h>

#include "context.h"
#include "memory.h"
#include "pointer_list.h"

pointer_list_t* context_stack = NULL;

void init_context(void) {

    if(context_stack == NULL)
        context_stack = create_pointer_list();
}

context_t* create_context(void) {

    context_t* ptr = _ALLOC_DS(context_t);
    ptr->table = create_hashtable();
    ptr->prev = peek_context();
    push_context(ptr);

    return ptr;
}

void push_context(context_t* ptr) {

    push_pointer_list(context_stack, ptr);
}

context_t* pop_context(void) {

    return pop_pointer_list(context_stack);
}

context_t* peek_context(void) {

    return peek_pointer_list(context_stack);
}


// actually stores objects of type ast_node_t*
void add_context_item(const char* name, void* node) {

    // check if symbol has already been defined or not.
    context_t* ptr = peek_context();
    insert_hashtable(ptr->table, name, node);
}

void* seek_context_item(const char* name) {

    context_t* ptr = peek_context();
    void* data = NULL;

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

