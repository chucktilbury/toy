
TODO:
* Functions that return a value but don't have a "return()" should publish error.
* Add formatted string introspection to detect unusable format options.
* Add TEXT data type for multi-line text definition.
* Either reorganize output so that all definitions are accesible or require that a symbol be defined before it is accessed.

-------------

Garbage collection

Note that it's okay to place requirements on the code that allocates memory because this is generated code. Any mechanism that makes GC easier or more efficient is permissable.

The garbage collector works with the notion of "context". The context of the memory allocation is tracked and the sweep function uses it to mark accessible memory. In the example of a simple function, all memory allocated by it is marked as inaccesible when the function returns, unless the pointer was returned by the funciton.

When a pointer is allocate in the context of another data structure, then the context of that data structure is the context of the pointer. The sweep function searches the data block that was allocated for pointers and they receive the same status as the root pointer.

All memory allocations are kept in a double-link list. When a function is entered a marker is dropped on the list that indicates where the context starts. Root allocations are allocated by one funciton and child allocations are allocated by another, where the root is given as a parameter. When the function returns the memory allocated by it is freed, unless it is returned by the funciton. In that case, the allocation becomes part of the caller's context. When a root allocation is freed, so are the children.

When the function modifies a pointer, such as doing pointer arithmatic, the compiler needs to set up a local so that the original pointer is no modified.

```
// garbage collection API

typedef struct _gc_node_t_ {
    unsigned long size;
    struct _gc_node_t_* next;
    struct _gc_node_t_* prev;
    struct _gc_node_t_* child;
    unsigned char buffer[1];
} GC_node_t;

// Call this before using any GC_* functions. Creates internal data
// structures, which are global to the program. There is only one heap.
void GC_init(void);

// Optional function to call at the end of the program. Normally all of
// the allocated memory will be cleaned up by the operating system, but
// this does it manually.
void GC_uninit(void);

// Call this as the first thing in a function. Assign the return value to a
// local and then use that when calling GC_end().
GC_node_t* GC_begin(void);

// Call this just before a return(); The first parameter is the allocation
// context. The second parameter is the intended return value, that will be
// passed to the return operator. If there is no allocated memory that will
// be returned, then the second parameter is NULL. For example when returning
// something like an INT.
void GC_end(GC_node_t*, void*);

// Allocate a root node. This always clears memory and there is no calloc()
// equivalent call. That is simulated with macros.
void* GC_alloc(size_t);

// Reallocate a node. can be either root or child and status is not changed.
void* GC_realloc(void*, size_t);

// Allocate a child node. i.e. the first parameter is a data structure
// (aka root) and the returned value is referred to by it.
void* GC_alloc_child(void*, size_t);

// Explicitly free an allocation and all of its children. Normally not
// required or used.
void GC_free(void*);

```

-------------

The difference between the parser and the AST is that the parser is postfix
and the AST is infix.

----------

TEXT blocks
Name spaces.

----------

In symtab.c, make a func to display the decorated name as a prototype for
errors.

---------
The raise statement has 2 args. The first one is a definition of the actual
exception. Internally, it is an enum and is used to specify which exception is
to be caught. The second argument is a formatted string. The string is
evaulated when the raise takes place and the resultant value is placed in the
arg slot, which is taken to be a string literal.

So in a raise(), the first arg is a definition and the second arg is a
formatted string.

In an except() the first arg is a reference and the second arg is a definition
of a string literal.

In the final except() if its present, the single arg is a definition, same as
the second arg of a regular except.

The enum value of a raise statement is always global and has it's own symbol
type.

-------

Symbol table is created in the parser and supported by the symtab.c module.
The context is tracked as a stack. When a nre context is pushed, the symbols
are create on that context. When the context is popped, then the symbols are
created on the previous context. The context parent is used when finding the
definition for a symbol.
