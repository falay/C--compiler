# C-- Compiler

Introduction
------------
The implementation of the compiler of C--, the subset of C language :
*  Implement 4 phases of a general compiler. 
*  Generates java bytecode running on JVM.

Compatibility
-------------
C-- compiler is capable of the following features:
*  Support integer and floating number declaration and computaion.
*  Support assignment, print, read, function call statement.
*  Support control flow statement, for and while.
*  Support function declaration and definition.

Usage 
-------------
```
make
```
or
```
make clean all
```
to compile the source

```
./parser [file.c] [target.j]
```
It generates java bytecode which is a stack machine code,
using jasmin package to generate the assembly code from the bytecode.

Script to run
```
make
sh runner.sh target.j
```
