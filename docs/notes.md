
TODO:
* Implement memory
* Implement objects
    * Function objects have an arity, an entry pointer, a prototype string, and then function parameters as attributes. In that order.
* Decide whether objects cast into their own type or out of their own type.
* Implement call stack
* Functions that return a value but don't have a "return()" should publish error.
* Add formatted string introspection to detect unusable format options.
* Add TEXT data type for multi-line text definition. Use for web templates for example.
* Either reorganize output so that all definitions are accessible or require that a symbol be defined before it is accessed.

Observation:
* The function parameters and the formatted strings parameters are actually a ``tuple``. This can be its own object type and use similar semantics as in Python.

-------------

## Object model

The basic unit of data in Toy is the object. All data and functions are bound to an object. When a function is defined as a global, it is bound to the root object, which is nameless.

### Class definitions

Classes are the prototype for an object. They contain only declarations and no initialization is allowed. A class may contain:

* Zero or more scope operators of ``public``, ``private``, and ``protected``.  Default scope is always private.
* Zero or more data declarations in the form of ``type_name identifier``. The type name can be any accessible name that names a type, including other classes.
* Zero or more method declarations. Method definitions are not allowed in a class definition.

Example
```
class base_class {
    public ; public scope until specified otherwise
    int data_var_1
    virtual nothing method(int)
    nothing method(string)
    private
    int data_var_2
}

; the word "base" is not accessible from the outside but is from within
; the child_class.
class child_class(private base_class as base) {
    public
    ; must be declared
    operator to_string
    virtual nothing method(int)
}

nothing base_class.method(string val) {
    ; convert val to a float and raise 10 by it
    ; if val cannot be converted then a runtime error is raised.
    ; if the result of the cast to int is too big, then a runtime error.
    data_var_2 = int(10^float(val))
    ; contrived
    data_var_1 = data_var_2
}

; implement the virtual function
nothing child_class.method(int val) {
    this.base.method(string(val))
}

; implement the to_string operator, which overrides the default, which
; simply returns "class child_class".
string child_class.to_string() {
    ; calls the string literal operator format()
    return("class child_class: %d"(base.data_var_1))
}

```

### When an object is constructed

Objects are constructed using the syntax that is given by the user. There is no functionality that allows methods or attributes to be added to an object during run time.

* The parent objects are added to the object, according to the scope operators.

    * Public means that the method or attribute and be seen externally. Note that an inherited class can be deemed public or private and this overrides the scope given in the inherited class to make a class item more private, but not more public.

    * Private means that a class item cannot be seen outside of the class, even if it was inherited as public. If a child class overrides a private method in the parent class, then functions in the inherited class can still use the private function and the child function will not know about the private function in the inherited class.

    * Protected means that the child class can see and use the attribute or method as if it was defined locally, but it is private to deny outside access.

* The default handlers for the operators and the default methods are added to the object by the default constructor. For example, the to-string cast operator is added and for integer literals, the to-float cast operator is added.

* User attribute and method definitions are added to the class definition at compile time.

* Operators that are not implemented for the class cause a runtime error to be generated.

* After that the operator overrides are added to the class, and then the user-defined methods are added.

### Accessing objects.

###### (not sure about the numbering thing. maybe I just need to use the names)
When a class is constructed into an object, the attributes and methods are given a number which corresponds to an index into an array of objects. The compiler remembers these numbers and uses them to build the code that accesses them. Once an object or method has been created, it cannot be deleted or moved. That means that operations such as monkey patching cannot be used. I believe that patching the code at runtime is inappropriate.

#### Data structure
```
// Note that this is for Simple, not Toy.
typedef struct _object_list_t_ {
    struct _object_t_** list;
    size_t capacity;
    size_t count;
} object_list_t;

// generic data object struct types
typedef enum {
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_STR,
    OBJ_BOOL,
    OBJ_INT_LIT,
    OBJ_FLOAT_LIT,
    OBJ_STR_LIT,
    OBJ_BOOL_LIT,
    OBJ_FUNC,
    OBJ_CLASS,
    OBJ_NAMESPACE,
    OBJ_USER,
} object_type_t;

// support generic re-casting
typedef struct _object_node_t_ {
    object_type_t type;
    size_t line_no;     // line where this object was encountered in user source
    const char* fname;  // file name in user source
    const char* name;   // name of object for error msgs

    // state of this node
    bool is_const;  // if true the value cannot be assigned
    bool is_init;   // if false the value has never been assigned
} object_node_t;

// Object data structure for an integer
// All of these names are reserved for classes. Needs to contrive better names.
typedef struct _int_object_t_ {
    object_node_t node;

    int64_t value;

    void (*set_int)(int64_t);
    void (*set_float)(double);
    int64_t (*get)(void);

    int64_t (*add)(int64_t);
    int64_t (*sub)(int64_t);
    int64_t (*mul)(int64_t);
    int64_t (*div)(int64_t);

    // other methods and operators that apply to an integer
    // for example comparisons

    const char* get_type(void);
    const char* to_string(void);
    double to_float(void);
    bool to_bool(void);

} rt_int_object_t;

```

#### Example

All user-defined classes and native types are handled exactly the same. Some methods or operators can be overridden at compile time. No "monkey patching" or overriding an existing method is allowed. Overriding virtual methods follows strict rules and is done completely at compile time.

All data in Toy (and in Simple) is an object, including literal values. For example, to add 2 literal numbers, the binary operator is implemented as a method call, such as ``foo = bar + 456`` would translate to something like:

```
// highly contrived example
rt_int_object_t* TOY_bar = create_int_object();

// source "test1.toy" 12 "foo = bar + 456"
rt_int_object_t* TOY_foo = create_int_object();

// runtime error because TOY_bar was never assigned a value
TOY_foo->set_int(TOY_bar->add(456));

```

When an object is created as an integer variable, operators are created for all operations that can be performed on it. In addition to getting and setting the value, converting it to a string or a float are also instantiated and given well-known names which the compiler references to implement the user's source code. The data structures that implement things like the integer and float intrinsic data types are hard coded with the data and methods that are needed to perform operations on them. User defined types have the user's operations defined in addition to certain default operations such as retrieving type information and converting to a string.


### This approach allows several things to happen.

* All operators are handled in the same manner, no matter what kind of object is being operated upon. If a particular operator is not implemented for a particular object, then that can be reasonably handled as a runtime error. If the user wants to define an operator for his object then that can be correctly handled at the syntax level.

* All objects that are not returned by a method or retained by the object are destroyed when the method returns, creating perfect GC for the user's application.

* Simple will not allow "monkey patching" for attributes or methods. All of them must be defined in the class and implemented. However, implementing virtual functions becomes easy to do because there is a slot defined for it.

### Inheritance

Multiple inheritance is supported by simply adding the base class to the child class by name. Virtual functions in the base class may or may not be implemented by the child. If a virtual function is called without an implementation, then that is a runtime error. If a deep child re-implements a virtual method, then only the new implementation is accessible. If a child class implements a non-virtual method, then that is a runtime error because making it a syntax error would require a lot of analysis that is easy at runtime. A non-virtual method that has no implementation is a syntax error.

Constructors are executed in the order they are declared in the class definition. Destructors are executed in the opposite order. The user has no control over it.

The public, private, and protected keywords are respected similar to C++.

### Implementation

In practice, the compiler will assign literal indexes to objects whenever it's possible. This makes using a hash table to store actual names unnecessary. The object creator understands the object type and calls a constructor specifically created for that type. In the case of a user defined type, there is a default constructor that builds the bare objects and adds the user-defined attributes and methods, and the user defined constructor is called after that. Note that Simple supports method overloading by parameter type, so there could be several constructors and the compiler will have to use the correct one. A default destructor is also created by the object builder that frees the memory associated with the object. The user defined destructor, if any, is called before that.

NOTE: Maybe I need an "override" operator, similar to virtual. But I don't think that native types should be possible to override methods or operators.

### Methods

A method object is the same as a data object in that it has attributes. The attributes are
* Arity. The number of parameters that the method has.
* An entry pointer. This is the actual pointer to call to enter the function.
* A prototype string. This is used by the runtime for error handling and for verifying that the function call is valid.
* A list of objects that are read-only parameters to the function.

A method is simply a function that operates on an object. The symbols that are defined for the class that implements the object are accessible by methods defined by the class by prefixing the access with the ``this`` keyword. That allows a method to unambiguously use the same name in the local scope as a variable defined by the class. Accessible variables in a base class are accessed using the name that is defined for it; i.e. there is no ``super`` keyword.

Methods can be overridden, similar to C++, by providing different parameter types. The actual name is then decorated using the type names to differentiate the functions as individuals.

#### Operators

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
    size_t size;
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
