/**
 * @file memory.c
 *
 * @brief Implement memory allocation.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-07
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include <stddef.h>
#include <string.h>

#include "runtime.h"
#include "rt_memory.h"
#include "common_errors.h"

/*
 * Garbage collection
 *
 * Note that it's okay to place requirements on the code that allocates memory
 * because this is generated code. Any mechanism that makes GC easier or more
 * efficient is permissable.
 *
 * The garbage collector works with the notion of "context". The context of
 * the memory allocation is tracked and the sweep function uses it to mark
 * accessible memory. In the example of a simple function, all memory
 * allocated by it is marked as inaccessible when the function returns, unless
 * the pointer was returned by the function.
 *
 * When a pointer is allocate in the context of another data structure, then
 * the context of that data structure is the context of the pointer. The sweep
 * function searches the data block that was allocated for pointers and they
 * receive the same status as the root pointer.
 *
 * All memory allocations are kept in a double-link list. When a function is
 * entered a marker is dropped on the list that indicates where the context
 * starts. Root allocations are allocated by one function and child allocations
 * are allocated by another, where the root is given as a parameter. When the
 * function returns the memory allocated by it is freed, unless it is returned
 * by the function. In that case, the allocation becomes part of the caller's
 * context. When a root allocation is freed, so are the children.
 *
 * When the function modifies a pointer, such as doing pointer arithmetic, the
 * compiler needs to set up a local so that the original pointer is no
 * modified.
 *
 */

#define TO_VOID(p)  ((void*)(((uint64_t)(p))+sizeof(rt_gc_node_t)))
#define TO_PTR(p)   ((rt_gc_node_t*)(((uint64_t)(p))-sizeof(rt_gc_node_t)))
rt_gc_node_t* root = NULL;
rt_gc_node_t* top = NULL;

#define _MALLOC malloc
#define _REALLOC realloc
#define _FREE free

void* _mem_alloc(size_t size) {

    void* ptr = _MALLOC(size);
    if(ptr == NULL)
        FATAL("cannot allocate %lu bytes", size);

    memset(ptr, 0, size);
    return ptr;
}

void* _mem_realloc(void* optr, size_t size) {

    void* nptr = _REALLOC(optr, size);
    if(nptr == NULL)
        FATAL("cannot re-allocate %lu bytes", size);

    return nptr;
}

void* _mem_copy(void* optr, size_t size) {

    void* nptr = _MALLOC(size);
    if(nptr == NULL)
        FATAL("cannot allocate to copy %lu bytes", size);

    memcpy(nptr, optr, size);
    return nptr;
}

char* _mem_copy_string(const char* str) {

    size_t len = strlen(str) + 1;
    char* ptr  = _MALLOC(len);
    if(ptr == NULL)
        FATAL("cannot allocate %lu bytes for string", len);

    memcpy(ptr, str, len);
    return ptr;
}

void _mem_free(void* ptr) {

    if(ptr != NULL)
        _FREE(ptr);
}

static void destroy_tree(rt_gc_node_t* node) {

    if(node == NULL)
        return;
    else if(node->child != NULL)
        destroy_tree(node->child);
    else if(node->next != NULL)
        destroy_tree(node->next);

    _FREE(node);
}

static void sweep_tree(rt_gc_node_t* node) {

    if(node == NULL)
        return;
    else if(node->inuse == false) {
        if(node->child != NULL) {
            sweep_tree(node->child);
            node->child = NULL;
        }
        else if(node->next != NULL) {
            sweep_tree(node->next);
            node->next = NULL;
        }

        _FREE(node);
    }
}

static void push_node(rt_gc_node_t* node) {

    node->next = top;
    top = node;
}

#if 0
static rt_gc_node_t* pop_node(void) {

    if(top != NULL)
        top = top->next;
    else
        rt_error("GC stack under-run");

    return top;
}

static rt_gc_node_t* peek_node(void) {

    if(top != NULL)
        top = top->next;
    else
        rt_error("GC stack internal error");

    return top;
}
#endif

//-----------------------------------------------------------------------------
// garbage collection API

/**
 * @brief Call this before using any GC_* functions. Creates internal data
 * structures, which are global to the program. There is only one heap.
 *
 */
void rt_gc_init(void) {

    root = RT_ALLOC_TYPE(rt_gc_node_t);
    root->size = 0;
    top = root;
}

/**
 * @brief Optional function to call at the end of the program. Normally all
 * of the allocated memory will be cleaned up by the operating system, but
 * this does it manually.
 *
 */
void rt_gc_uninit(void) {

    destroy_tree(root);
}

/**
 * @brief Call this as the first thing in a function. Assign the return value
 * to a local and then use that when calling GC_end().
 *
 * @return rt_gc_node_t*
 */
rt_gc_node_t* rt_gc_begin(void) {

    return top;
}

/**
 * @brief Call this just before a return(); The first parameter is the
 * allocation context. The second parameter is the intended return value, that
 * will be passed to the return operator. If there is no allocated memory that
 * will be returned, then the second parameter is NULL. For example when
 * returning something like an INT.
 *
 * @param node
 * @param ptr
 */
void rt_gc_end(rt_gc_node_t* node, void* ptr) {

    rt_gc_node_t* tmp = TO_PTR(ptr);

    tmp->inuse = true;
    sweep_tree(node);
    tmp->inuse = false;

    push_node(tmp);
}

/**
 * @brief Allocate a root node. This always clears memory and there is no
 * calloc() equivalent call. That is simulated with macros.
 *
 * @param size
 * @return void*
 */
void* rt_gc_alloc(unsigned long size) {

    // cleared memory
    void* p = RT_ALLOC(size+sizeof(rt_gc_node_t));
    return TO_VOID(p);
}

/**
 * @brief Reallocate a node. can be either root or child and status is not
 * changed.
 *
 * @param ptr
 * @param size
 * @return void*
 */
void* rt_gc_realloc(void* ptr, unsigned long size) {

    void* v = (void*)TO_PTR(ptr);
    rt_gc_node_t* p = RT_REALLOC_TYPE(v, rt_gc_node_t, size);

    return TO_VOID(p);
}

/**
 * @brief Allocate a child node. i.e. the first parameter is a data structure
 * (aka root) and the returned value is referred to by it.
 *
 * @param ptr
 * @param size
 * @return void*
 */
void* rt_gc_alloc_child(void* ptr, unsigned long size) {

    rt_gc_node_t* p = TO_PTR(ptr);
    p->child = rt_gc_alloc(size);
    return TO_VOID(p->child);
}

/**
 * @brief Explicitly free an allocation and all of its children. Normally not
 * required or used.
 *
 * @param ptr
 */
void rt_gc_free(void* ptr) {

    (void)ptr;
}
