
Make sure that global initializes all have values.

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
