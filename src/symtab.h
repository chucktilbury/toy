/*
 *  Symbol table class.
 *
 *  This class implements the symbol tables for TOI. It consists of a tree
 *  structure has a left, right, child, and parent pointer. The root of a tree
 *  represents a complete symbol scope. Each leaf of the tree and all of its
 *  nodes have a pointer to the root of the scope context. This is used when
 *  we have a leaf and we want to find another symbol in the tree. The tree is
 *  traversed from the root. The root node has a pointer back to the parent of
 *  its symbol context. If the symbol is not found in the current tree, then the
 *  parent is searched and so on. The child pointer points to another symbol
 *  context that is defined as a child of the current context.
 *
 *  When a symbol is added, for example a method in a class, the symbol is
 *  placed as a leaf in the class context, but it a child is created for it
 *  when its added. All symbols that are defined for the method are then created
 *  in the child.
 *
 *  Symbols are added from the root. The root (or the child, depending on your
 *  point of view) is then passed around as the current symbol context.
 *
 *  For example, suppose I want to look up a symbol such as "one.two.three". I
 *  will look for "one" in the current context. If it is not found there, then
 *  I will look in the parent, and if not found, in that parent and so on until
 *  I either reach the root with no parent. If it is not found, there, then it
 *  has not been defined. If it is found, then "two" must be defined in the
 *  context of "one", either in a child or as a symbol in the same context. If
 *  it is not there, then "two" has not been defined. Lastly, if "two" is found,
 *  then it must have a child context. If it does not  then "three" has not
 *  been defined. If "three" is not found in the immediate child context, then
 *  it has not been defined.
 *
 *  The first "one" that is located is taken to be the correct definition. This
 *  symbol could be defined in other contexts without a name collision.
 *
 *  Anonymous symbol contexts are those such as inside a "for" loop. Vars can be
 *  defined there that are only in the scope of the block. Anonymous symbol
 *  contexts are handled the same as named symbols, except that they are looked
 *  up differently if they are defined in an anonymous context. In an anonymous
 *  context, lookup can leave the context, but it cannot enter it. Take the
 *  "one.two.three" example where one is defined in the class with the method
 *  that has the reference to the symbol. like this:
 *
 *  class someName {
 *    someOtherClass one;
 *    method methodName(input)(output) {
 *      if(true) {
 *         uint8 x = one.two.three();
 *      }
 *      # cannot see "x" here.
 *    }
 *  }
 *
 *  "one" refers to the class, which has "two" and "three" defined as it wants,
 *  to return a uint8. If an attempt to access "x" happens outside and after the
 *  if(), then the context of the if() is skipped when the lookup happens and
 *  the symbol will not be found. The lookup goes straight to the table for
 *  "methodName" and cannot enter the anonymous context.
 *
 *  To facilitate faster lookups, a hash is also stored with the name because a
 *  number is faster to compare than a string is. If the hash matches, then the
 *  string is checked, otherwise, not.
 *
 *  Symbols are "decorated" according to their type. (tentative)
 *  1.  Classes are decorated with the names that they inherit. For example,
 *      this definition "class name(public other, private another)" would come
 *      out as "name@other$another", where the "@" indicates a public name and
 *      the "$" indicates a private name. Also use "#" for a protected name.
 *      Note that since these names are used raw, and never parsed, there is no
 *      conflict with things like comments.
 *
 *  2.  Methods are decorated with the input and output parameter types. For
 *      example the definition "method name(uint8 flag, int num)(int retv)"
 *      would get decorated as "name@uint8@int#int" to signify a unique name.
 *      This way you can define the same method name and override it to return
 *      different types.
 *
 *  Decorated symbols are stored with their decorated names as well as their
 *  local names, each with a hash. (all symbols are decorated?)
 *
 */
#ifndef _SYMTAB_H_
#define _SYMTAB_H_

// TODO: resolve reference vs definition where types are concerned. I.e. is an
// inheritance spec a ref or a def? How does that effect the symbol table?

// TODO: Think about how tokenizing a block works with reference to the symbol
// table.

// NTS: begin executing code as soon as possible.

typedef enum
{
    CLASS_NAME,
    METHOD_NAME,
    ANONYMOUS_CONTEXT,
    STRING_TYPE,
    UINT16_TYPE,
    UINT32_TYPE,
    UINT64_TYPE,
    UINT8_TYPE,
    UINT_TYPE,
    INT16_TYPE,
    INT32_TYPE,
    INT64_TYPE,
    INT8_TYPE,
    INT_TYPE,
    FLOAT_TYPE,
    BOOL_TYPE,
    HASH_TYPE,
    ARRAY_TYPE,
} _symbol_type_t;

typedef enum
{
    NO_ATTR         = 0x00000000,
    STATIC_ATTR     = 0x00000001,
    PUBLIC_ATTR     = 0x00000002,
    PRIVATE_ATTR    = 0x00000004,
    PROTECTED_ATTR  = 0x00000008,
    CONST_ATTR      = 0x00000010,
    LOCATE_ATTR     = 0x00000020,
    BLOCK_ATTR      = 0x00000040,
} _symbol_attrib_t;

#include <string>
using namespace std;

class symbolTable
{

public:
    symbolTable();
    ~symbolTable();

    // creates symbol and makes it the crnt one.
    void new_symbol(string name, int type);

    // these methods move the crnt_context around.
    void up_context(void);   // move to the parent context
    void down_context(void); // move to the child context

    // these methods work on the crnt_symbol
    void decorate_symbol(string name, ...);
    void assign_value(unsigned long long num);
    void assign_value(long long num);
    void assign_value(double num);
    void assign_value(string num);
    void assign_value(string* num);

    // get the caller accessible state
    // find a symbol based on the name this is almost always be the decorated name
    bool resolve_symbol(string name);
    _symbol_type_t symbol_type(void);
    _symbol_attrib_t symbol_attrib(void);
    unsigned long long symbol_unum(void);
    long long symbol_snum(void);
    double symbol_fnum(void);
    string* symbol_str(void);

private:
    // This struct contains the entire state of any possible symbol.
    struct node
    {
        // pointers to other nodes
        struct node* left;
        struct node* right;
        struct node* parent;
        struct node* child;

        // This is the type of thing that the symbol represents. See the
        // types enum. I.e. a uint32, an anonymous context, or a class name.
        _symbol_type_t type;
        string* name;
        unsigned int hash;
        _symbol_attrib_t attrib;

        // Note: will want to put file management in this as soon as possible.
        // Blocks will have the file name, with the file index of the beginning
        // of the block and the end.

        // most symbols do not have a value, such as keywords and names of
        // methods. Note that literal strings and numbers are only useful in the
        // context of being assigned somehow.
        union value
        {
            unsigned long long unum;
            long long snum;
            double fnum;
            string* str;
        };
    };

    // these represent the state of the symbol table, rather than a particular
    // symbol.
    struct node* root; // This is absolute root of the table.
    struct node* crnt_context; // the context where new symbols will be added.
    struct node* crnt_symbol;  // the the most recently added symbol.
};

#endif /* !_SYMTAB_H_ */
