#ifndef _RT_MEMORY_
#define _RT_MEMORY_

#include <stddef.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// System memory wrapper for C runtime
#define RT_ALLOC(s) rt_mem_alloc(s)
#define RT_ALLOC_TYPE(t) (t*)rt_mem_alloc(sizeof(t))
#define RT_ALLOC_ARRAY(t, n) (t*)rt_mem_alloc(sizeof(t) * (n))
#define RT_REALLOC(p, s) rt_mem_realloc((void*)(p), (s))
#define RT_REALLOC_TYPE(p, t, s) (t*)rt_mem_realloc((void*)(p), (s))
#define RT_REALLOC_ARRAY(p, t, n) (t*)rt_mem_realloc((void*)(p), sizeof(t) * (n))
#define RT_COPY(p, s) rt_mem_copy((void*)(p), (s))
#define RT_COPY_DS(p, t) (t*)rt_mem_copy((void*)(p), sizeof(t))
#define RT_COPY_ARRAY(p, t, n) (t*)rt_mem_copy((void*)(p), sizeof(t) * (n))
#define RT_COPY_STRING(s) rt_mem_copy_string(s)
#define RT_FREE(p) rt_mem_free((void*)(p))

void* rt_mem_alloc(size_t);
void* rt_mem_realloc(void*, size_t);
void* rt_mem_copy(void*, size_t);
char* rt_mem_copy_string(const char*);
void rt_mem_free(void*);

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


#endif /* _RT_MEMORY_ */
