#ifndef _RT_MEMORY_H_
#define _RT_MEMORY_H_

#include <stdbool.h>

//-----------------------------------------------------------------------------
// garbage collection API
// GC node data structure.
typedef struct _gc_node_t_ {
    unsigned long size;
    bool inuse;
    struct _gc_node_t_* next;
    struct _gc_node_t_* prev;
    struct _gc_node_t_* child;
    unsigned char buffer[1];
} rt_gc_node_t;

// Call this before using any GC_* functions. Creates internal data
// structures, which are global to the program. There is only one heap.
void rt_gc_init(void);

// Optional function to call at the end of the program. Normally all of
// the allocated memory will be cleaned up by the operating system, but
// this does it manually.
void rt_gc_uninit(void);

// Call this as the first thing in a function. Assign the return value to a
// local and then use that when calling GC_end().
rt_gc_node_t* rt_gc_begin(void);

// Call this just before a return(); The first parameter is the allocation
// context. The second parameter is the intended return value, that will be
// passed to the return operator. If there is no allocated memory that will
// be returned, then the second parameter is NULL. For example when returning
// something like an INT.
void rt_gc_end(rt_gc_node_t*, void*);

// Allocate a root node. This always clears memory and there is no calloc()
// equivalent call. That is simulated with macros.
void* rt_gc_alloc(unsigned long);

// Reallocate a node. can be either root or child and status is not changed.
void* rt_gc_realloc(void*, unsigned long);

// Allocate a child node. i.e. the first parameter is a data structure
// (aka root) and the returned value is referred to by it.
void* rt_gc_alloc_child(void*, unsigned long);

// Explicitly free an allocation and all of its children. Normally not
// required or used.
void rt_gc_free(void*);


#endif /* _RT_MEMORY_H_ */
