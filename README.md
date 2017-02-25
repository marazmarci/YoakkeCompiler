# What is Yoakke?
Yoakke is a general-purpose, system-level programming language aiming for performance and safety, while still providing tools for modern programming techniques. While this may sound amazing, the official compiler is developed by a single person, so there is no fast development or rapid updates for it. It started out as a learning project but hopes to grow something bigger.

## Status of development
 * Lexical analysis: done
 * Parsing syntax: done
 * Semantic analysis: in progress
 * IR generation: not started
 * Optimizations: not started
 * Compilation: not started

___
# Getting started

## Hello, World!
```
#import "io"

main :: () {
    io:println("Hello, World!");
}
```
The sintax is not your usual, but pretty simple. #import is a compiler directive that loads a module (in this case a module called 'io'). The double colon means static assignment. In this example, we assign a function to the symbol 'main'. The function body contains a single statement, which is to call println from the namespace/module 'io'.

Main is a function that returns a type 'unit', which is the equvivalent to the type 'void' in some languages. It can be noted with the keyword 'unit', or empty parenthesis. If you don't specify the return type explicitly, it will default to unit. You can explicitly specify the return type after the arrow:
```
main :: () -> () {
}
```
