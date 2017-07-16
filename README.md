# TOI 
(pronounced "toy")

This is the TOI programming language. TOI stands for Toy Object Interpreter. It is a synthesis of C and Python. The syntax is much like C, but the object model is much like Python. The language has strong types, but all of the types are implemented using objects. The language tries to be as simple as possible, but still complete. The philosophy is to use libraries rather than language features when possible. For example, take these C++ and C statements:

```c++
std::cout << "This is a number: " << num << "and a string: " << strg << std::endl;
printf("This is a number: %d and a string: %s\n", num, strg);
```

They are both functionally identical, but the C++ statemnt uses operator overloading, which is a feature of the language. The C statement uses a library to substitute the values in the string. It's a matter of where to put the complexity. TOI places it in a library.

TOI is "strongly typed" instead of "duck typed" like Python. This is a simpler solution and (for me at least) it should make the execution of code faster.

## What does TOI look like?
TOI looks a lot like C. Here is a high level example:
```C++
# This statement brings in the class and its symbols into this context 
# so that it can be referenced using the optional alias name. There is 
# search path for classes that are imported and the directories are 
# optional.
import dir1.dir2.file.class as name

# A class definition
class ClassName(AnotherClass) {
   uint8 variable;
   
   # create and destroy methods are mandatory, but may be empty.
   # create and destroy must be public.
   create(int var1) {
       # there are no output parameters with the constructor.
       # all parameters must be referenced.
   }
   create(int var1, int var2) {
       # this is a different constructor than the one above.
   }
   destroy {
       # close any files and whatnot. Causes the other objects that
       # were created by this one to have their destroy methods called.
       # There are no parameters on the destroy method.
   }
   
   # this overrides a method with the same name in AnotherClass
   method methodName(int var1, string var2)(int return_value) {
       # this disables the input variable. All inputs and outputs
       # must be referenced.
       nothing(var1); 
       int32 something; # a reasonable type compatibility
       AnotherClass.methodName(var2)(something);
       if(something == 0) {
           # do something
       }
       else {
           # do something else
       }
   }
}
```
Notice the notion of input and output parameters. Input parameters are passed to a method by value. They cannot be modified by a method directly. Output parameters are passed to the method by address. They are read and written exactly as if they were defined locally, but they retain their value outside of the calling method. Methods are not allowed in expressions. 

Scope operators of public, private, and protected have their normal functions. The default scope of a method is public and the default scope of a variable defined in a class is private. 

Storage operator static works as one would expect. A static method can have exactly one implementation and may not be overridden. A static variable has exactly one location in memory and may not be "shadodwed". 

## Internals
TOI is an interpreted language. Interpretation is done in two passes. There is a definition pass and a reference pass. I call them defs and refs. The first pass creates the symbol tables and verifies the syntax of classes and method definitions. The second pass actually executes the code, or references the symbols.

### Symbols (defs)
Symbols are held in a data structure (called a symbol table) that allows quickly searching and verifying a symbol for scope and type. When a '{' is encountered, a new symbol context (AKA scope) is created. When a symbol is resolved, first the most local context is examined, then more remote ones. A reference to where the symbol is located in stored in the symbol table as well as the file index of the symbol so that when the second pass takes place, the file does not need to be scanned again. Method bodies are tokenized and stored in memory for execution, but no execution takes place until the second pass.

Symbol tables are, for the most part, an unbalanced binary tree. Some symbols such as class names have their own symbol tables, so a symbol table structure is a part of every symbol. Every symbol definition must have a reference back to the root of its tree. When a symbol is referenced, searching for it starts at the root of the tree. If it's not found, then the next tree up is searched. If it's not found in the root tree, then the symbol has not been defined.

### Blocks (refs)
Blocks are areas that hold the actual program. What is meant by tokenizing is that keywords are reduced to their integer form and symbol names are stored as references to the symbol table. Expressions are broken up into parse trees and stored that way. Blocks of code (that is areas that are surounded by {}) are stored such that no calculation is required to skip them as in the case of an "if" statement. A tokenized block is basically a stream of integers that control a state machine where the actual execution takes place.

### Exceptions
Exceptions are handled as non-local GOTO statements. Different types of exceptions are functionally handled in much the same way as Python. Most of this is still TBD.

# Status
The current status of the project is still in the very early stages. I am still writing the parser and symbol table code. 

## Immediate goals
  * Finish defining the data structures for the symbol table.
  * Get the first pass of the parser completed.
  * Get the tokenizer part of the parser defined, then implemented.
  * testing.
  * testing.
  * more testing.

