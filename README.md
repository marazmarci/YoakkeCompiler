<img src="https://raw.githubusercontent.com/LPeter1997/YoakkeCompiler/master/ylogo.png" width="512">

# What is Yoakke?
Yoakke is a general-purpose, system-level programming language aiming for performance and safety, while still providing tools for modern programming techniques. While this may sound amazing, the official compiler is developed by a single person, so there is no fast development or rapid updates for it. It started out as a learning project but hopes to grow something bigger.

## Status of development
 * Lexical analysis: done
 * Parsing syntax: done
 * Semantic analysis: in progress
 * IR generation: in progress
 * Optimizations: not started
 * Compilation: in progress

___
# Basics

## Hello, World!
```
#import "io"

fn main = {
    io::println("Hello, World!");
}
```
The syntax is not your usual, but pretty simple. #import is a compiler directive that loads a module (in this case a module called 'io'). 

Main is a function that returns a type 'unit', which is the equvivalent to the type 'void' in some languages. It can be noted with the keyword 'unit', or empty parenthesis. If you don't specify the return type explicitly, it will default to unit. You can explicitly specify the return type after the arrow:
```
fn main = -> () {
}
```

## Variables
You can define variables using the 'let' keyword.
```
fn main = {
    let x: i32 = 4;  // Explicit type
    let x = 4u32;    // Implicit type with literal operator
    // let z = 4;    ERROR: 4 is not a specific type
}
```
As you can see, integers must have a specific type, as integer literals are always painful with type inference. Given literal operators and explicit type annotations hope to solve the problem.

TODO: Continue
