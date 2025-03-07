/**
 * @file context.c
 * 
 * @brief This manages the symbolic context. The actual forward or "next" 
 * pointer is located in the AST. The "prev" pointer is located in the context 
 * data structure. When a symbol is looked up, it is only searched from the 
 * current context toward the root, so the prev pointer if the one that 
 * matters for this data structure.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-02-28
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include <stdbool.h>

#include "context.h"
#include "memory.h"
#include "errors.h"

// #define TRACE_CONTEXT

#ifdef TRACE_CONTEXT
#define USE_TRACE
#endif
#include "trace.h"

static context_t* root_context = NULL;
static context_t* crnt_context = NULL;

/**
 * @brief Create a context object
 * 
 * @return context_t* 
 */
context_t* create_context(void) {

    context_t* ptr = _ALLOC_DS(context_t);
    ptr->table     = create_hashtable();
    ptr->prev      = peek_context();

    push_context(ptr);

    return ptr;
}

/**
 * @brief Push a context on the context stack.
 * 
 * @param ptr 
 */
void push_context(context_t* ptr) {

    if(root_context == NULL)
        root_context = ptr;

    TRACE("push context: %p", (void*)ptr);
    crnt_context = ptr;
}

/**
 * @brief Pop a context object and return the new top of stack.
 * 
 * @return context_t* 
 */
context_t* pop_context(void) {

    if(crnt_context == NULL)
        FATAL("symbolic context stack underrun");

    TRACE("pop context: %p", (void*)crnt_context);
    crnt_context = crnt_context->prev;
    return crnt_context;
}

/**
 * @brief Return the context object that is on the top of the stack without 
 * changing the stack.
 * 
 * @return context_t* 
 */
context_t* peek_context(void) {

    TRACE("peek context: %p", (void*)crnt_context);
    return crnt_context;
}

/**
 * @brief Set the current top of stack to the root object.
 * 
 * @return context_t* 
 */
context_t* reset_context(void) {

    if(root_context == NULL)
        FATAL("symbol context error: root context is NULL");

    TRACE("reset context: %p", (void*)root_context);
    crnt_context = root_context;

    return root_context;
}
