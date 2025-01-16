# toy
The toy programming language.

This is a simple programming language that I intend to expand over time. To start with, it is just a toy that is not really intended to be useful for general programming tasks. It's designed to be an exploration of the process of creating an operational tool chain for creating software.

The actual language is a simplified look at the C language, but with some added features.
* Function overloading. When a function name is created or referenced, the types of the parameters is a part of the name so that the name of the parameter types diferentiates it from functions with the same name, but different parameter types.
* Native data types
  * int -- actually a 64 bit signed long when processed.
  * float -- actually a double type.
  * string -- a native string. A simple wrapper for C strings.
  * list -- a list of things that can contain an arbitrary data type.
  * hash -- a hash table that can contain arbitrary data types.
  * file -- a file handle that is handled as a native data type.
* Global variables
* Exceptions
* Garbage collection.

Missing features.
* Data strucutres. No user defined types.
* Multiple files. The entire program must be in one file.
* Pointers. 

