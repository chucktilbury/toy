
TODO:
* Implement objects
* Functions that return a value but don't have a "return()" should publish error.
* Add formatted string introspection to detect unusable format options.
* Add TEXT data type for multi-line text definition. Use for web templates for example.
* Either reorganize output so that all definitions are accessible or require that a symbol be defined before it is accessed.

-------------

## Object model

The basic unit of data in Toy is the object. All data and functions are bound to an object. When a function is defined as a global, it is bound to the root object, which is nameless.

A function is another kind of object that has a parameter list as attributes and an entry point for execution. Objects are created and deleted inside a function. The return keyword allows the function to return an object that was created in the function.

All user-defined classes and native types are handled exactly the same. Some methods or operators can be overridden at compile time. No "monkey patching" or overriding an existing method is allowed. Overriding virtual methods follows strict rules and is done completely at compile time.

All data in Toy (and in Simple) is an object, including literal values. For example, to add 2 literal numbers, the binary operator is implemented as a method call, such as ``val = 123 + 456`` would translate to something like:
```
// objects[] is the object table for objects defined in this GC context.
int obj1 = create_object(OBJ_INT_LIT);
objects[obj1]->assign(123);
int obj2 = create_object(OBJ_INT_LIT);
objects[obj2]->assign(456);
int obj3 = create_object(OBJ_INT_VAR);
// there is a handler for every type of possible object that can be "add"ed. the compiler
// decides which one to call.
objects[obj3]->assign(objects[obj1]->add_int(objects[obj2]));

```

### This approach allows several things to happen.

* Operators are handled in the same manner, no matter what kind of object is being operated upon. If a particular operator is not implemented for a particular object, then that can be reasonably handled as a runtime error. If the user wants to define an operator for his object then that can be correctly handled at the syntax level.

* All objects that are not returned by a method or retained by the object are destroyed when the method returns, creating perfect GC for the user's application.

* Simple will not allow "monkey patching" for attributes or methods. All of them must be defined in the class and implemented. However, implementing virtual functions becomes easy to do because there is a slot defined for it.

### Inheritance

Multiple inheritance is supported by simply adding the base class to the child class. Virtual functions in the base class may or may not be implemented by the child. If a virtual function is called without an implementation, then that is a runtime error. If a deep child re-implements a virtual method, then only the new implementation is accessible. If a child class implements a non-virtual method, then that is a runtime error because making it a syntax error would require a lot of analysis that is easy at runtime. A non-virtual method that has no implementation is a syntax error.

Constructors are executed in the order they are declared in the class definition. Destructors are executed in the opposite order. The user has no control over it.

The public, private, and protected keywords are respected similar to C++.

### Implementation

In practice, the compiler will assign literal indexes to objects whenever it's possible. This makes using a hash table to store actual names unnecessary. The object creator understands the object type and calls a constructor specifically created for that type. In the case of a user defined type, there is a default constructor that builds the bare objects and adds the user-defined attributes and methods, and the user defined constructor is called after that. Note that Simple supports method overloading by parameter type, so there could be several constructors and the compiler will have to use the correct one. A default destructor is also created by the object builder that frees the memory associated with the object. The user defined destructor, if any, is called before that.

NOTE: Maybe I need an "override" operator, similar to virtual. But I don't think that native types should be possible to override methods or operators.

### Operators

An operator is a type of method defined on a class. All operators can be overridden by a user class at compile time, but not at run time. Besides the comparison and arithmetic operators, there are some additional ones.

* type-of: produces a string that depicts the type of the class, including a name in the case of a user-defined type.

* cast-to-xxx: attempts to return a representation of a native value that the cast is to. Since Simple is a strongly typed language, explicit casting is required. Casting to a string is supported by default for all classes.

* exec: provides an operator that allows the class to be executed as a method.

Of course, for user defined types the comparison and arithmetic operators are undefined.

-------------

## Garbage collection

This system uses deterministic garbage collection. This is possible because all of the code that enters and returns from a function is emitted by the compiler.

Note that it's okay to place requirements on the code that allocates memory because this is generated code. Any mechanism that makes GC easier or more efficient is permissible.

The garbage collector works with the notion of "context". The context of the memory allocation is tracked and the sweep function uses it to mark accessible memory. In the example of a simple function, all memory allocated by it is marked as inaccessible when the function returns, unless the pointer was returned by the function.

When a pointer is allocated in the context of another data structure, then the context of that data structure is the context of the pointer. The sweep function searches the data block that was allocated for pointers and they receive the same status as the root pointer.

All memory allocations are kept in a link list that acts like a stack. When a function is entered a marker is dropped on the list that indicates where the context starts. Root allocations are allocated by one function and child allocations are allocated by another, where the root is given as a parameter. When the function returns the memory allocated by it is freed, unless it is returned by the function. In that case, the allocation pushed on the caller's GC context stack. When a root allocation is freed, so are the children.

```
// garbage collection API

typedef struct _gc_node_t_ {
    char magic[8];
    unsigned long size;
    struct _gc_node_t_* child;
    struct _gc_node_t_* next;
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
evaluated when the raise takes place and the resultant value is placed in the
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
