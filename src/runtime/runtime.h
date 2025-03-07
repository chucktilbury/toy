#ifndef _RUNTIME_H_
#define _RUNTIME_H_

extern int runtime_error_number;

// initialize the runtime code
void runtime_main_init(int, char**, char**);

// clean up after program run
void runtime_main_uninit(void);

//-----------------------------------------------------------------------------
// strings API
// String data strucutre.
typedef struct _string_t_ {
    char* text;
    int len;
    int cap;
} runtime_string_t;

// returns an allocated string in garbage collected code.
runtime_string_t* runtime_string_format(const char*, ...);

// send the string to stdout.
void runtime_string_print(runtime_string_t*);


//-----------------------------------------------------------------------------
// garbage collection API
// GC node data structure.
typedef struct _gc_node_t_ {
    unsigned long size;
    struct _gc_node_t_* next;
    struct _gc_node_t_* prev;
    struct _gc_node_t_* child;
    unsigned char buffer[1];
} runtime_gc_node_t;

// Call this before using any GC_* functions. Creates internal data
// structures, which are global to the program. There is only one heap.
void runtime_gc_init(void);

// Optional function to call at the end of the program. Normally all of
// the allocated memory will be cleaned up by the operating system, but
// this does it manually.
void runtime_gc_uninit(void);

// Call this as the first thing in a function. Assign the return value to a
// local and then use that when calling GC_end().
runtime_gc_node_t* runtime_gc_begin(void);

// Call this just before a return(); The first parameter is the allocation
// context. The second parameter is the intended return value, that will be
// passed to the return operator. If there is no allocated memory that will
// be returned, then the second parameter is NULL. For example when returning
// something like an INT.
void runtime_gc_end(runtime_gc_node_t*, void*);

// Allocate a root node. This always clears memory and there is no calloc()
// equivalent call. That is simulated with macros.
void* runtime_gc_alloc(unsigned long);

// Reallocate a node. can be either root or child and status is not changed.
void* runtime_gc_realloc(void*, unsigned long);

// Allocate a child node. i.e. the first parameter is a data structure
// (aka root) and the returned value is referred to by it.
void* runtime_gc_alloc_child(void*, unsigned long);

// Explicitly free an allocation and all of its children. Normally not
// required or used.
void runtime_gc_free(void*);


#endif /* _RUNTIME_H_ */
