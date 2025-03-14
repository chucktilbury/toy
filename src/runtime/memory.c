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
#include "runtime.h"

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

//-----------------------------------------------------------------------------
// garbage collection API

/**
 * @brief Call this before using any GC_* functions. Creates internal data
 * structures, which are global to the program. There is only one heap.
 *
 */
void runtime_gc_init(void) {

}

/**
 * @brief Optional function to call at the end of the program. Normally all
 * of the allocated memory will be cleaned up by the operating system, but
 * this does it manually.
 *
 */
void runtime_gc_uninit(void) {

}

/**
 * @brief Call this as the first thing in a function. Assign the return value
 * to a local and then use that when calling GC_end().
 *
 * @return runtime_gc_node_t*
 */
runtime_gc_node_t* runtime_gc_begin(void) {

    runtime_gc_node_t* p = NULL;

    return p;
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
void runtime_gc_end(runtime_gc_node_t* node, void* ptr) {

}

/**
 * @brief Allocate a root node. This always clears memory and there is no
 * calloc() equivalent call. That is simulated with macros.
 *
 * @param size
 * @return void*
 */
void* runtime_gc_alloc(unsigned long size) {

    void* p = NULL;

    return p;
}

/**
 * @brief Reallocate a node. can be either root or child and status is not
 * changed.
 *
 * @param ptr
 * @param size
 * @return void*
 */
void* runtime_gc_realloc(void* ptr, unsigned long size) {

    void* p = NULL;

    return p;
}

/**
 * @brief Allocate a child node. i.e. the first parameter is a data structure
 * (aka root) and the returned value is referred to by it.
 *
 * @param ptr
 * @param size
 * @return void*
 */
void* runtime_gc_alloc_child(void* ptr, unsigned long size) {

    void* p = NULL;

    return p;
}

/**
 * @brief Explicitly free an allocation and all of its children. Normally not
 * required or used.
 *
 * @param ptr
 */
void runtime_gc_free(void* ptr) {

}
