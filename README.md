# DebugUtils

These are debugging utilities that print out variables and messages when enabled and are entirely removed
from your code when disabled.

The turning on/off of debugging code is driven by policy types that trigger template specialization and 
function overloading that generate debugging code when requested and no code when not requested.

Debugging code using policy types that drive template specialization and function overloading 
that generate debugging code when requested and no code not requested.  C++20 is required.

Debug code generation is selected by `#define DEBUGUTILS_ON` to be non-zero at compile time.  

## The Experiment

This code was an experiment to see how far modern C++20 tools can replace the preprocessor to conditionally 
generate code.  **It turns out I was able to entirely replace the preprocessor for this purpose.**

There is no preprocessor selected code generation in DebugUtils.  The selection of what code is generated 
(or not generated) is entirely governed by template specialization (helped along by the compiler's type inference) 
and function overloading.  Certain template specializations and function overloads 
generate debugging code; other template specializations and function overloads generate no code.

When compiled on GCC v13 with -O2 and `DEBUGUTILS_ON=0`, code size is exactly the same as when all 
DebugUtils debugging calls are manual editted out from the source code.

Variadic template functions allow as many variables to be output as desired.  Template recursion
enables the debug printing of complex nested data types and data structures (e.g., vectors of pairs, 
lists of vectors, maps of strings, queues of tuples).

A small number of preprocessor macros are provided to provide a way to conveniently capture `__FILE__` and 
`__LINE__` and produce a string concatenation of the arguments at the point of calling the debugging 
functions.  *If anyone knows a way to do this without using a preprocessor macro, please let me know!*  Note 
that none of these macros are conditional.  A final macro simply hides a simple but rote object 
instantiation (it is a macro of convenience, not of necessity). 

## Usage

All the DebugUtils code is contained in the header file `DebugUtils.hpp`.  The file `main.cpp` illustrates
how DebugUtils are used.  By default, debugging output is directed to `std::cerr`.  If you wish to send the
output to a file, instantiate an object of type `DebugUitls::DebugFileOn` and pass the constructor a file name.
This is illustrated in `main.cpp`.  Debugging output will be directed to the selected file until the object 
is destroyed (usually when it goes out of scope).  Debugging output reverts to `std::cerr` when that happens.

## Provenance

Parts of this code are adapted from code by Anshul Johri.  They did not provide any license or copyright info.
You can access their version at https://github.com/Anshul-Johri-1/Debug-Template

