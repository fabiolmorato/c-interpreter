# c-interpreter

This is an interpreter for a Lox-based language written in C. So far, it implements the following language:

```
logic_expression -> expression ( ( "and" | "or" ) expression )* ;
expression -> equality ( "," equality )* ;
equality -> comparison ( ( "==" | "!=" ) comparison )* ;
comparison -> addition ( ( ">" | ">=" | "<" | "<=" ) addition )* ;
addition -> multiplication ( ( "+" | "-" ) multiplication )* ;
multiplication -> unary ( ( "*" | "/" ) multiplication )* ;
unary -> ( ( "!" | "+" | "-" ) )* primary ;
primary -> NUMBER | "true" | "false" | "(" logic_expression ")" ;
```

### Compilation

To compile this program, run `make main`.

### Usage

Run this program with `./main` and type whatever arithmetic and logic expressions you want, as long as it is part of the language implemented. The following lines are examples of what you could type:

```
1 + 7 - 3 * (1 - 2 / (4 - 7)) - 3 / (9 * (1 + 3 * (2 - 9)) + 1)
3.5 / 9 + 1
(1 < 3 and 5 < 4) or (1 <= true and !(1 - 1))
```

The interpreter does implicit conversions. In cases where a number is expected but a boolean is given it it converted into a number and vice-versa.

### Credits

I'm writing this project based on [Crafting Interpreters](http://craftinginterpreters.com/) free book. It's a great book on this topic and I definitely recommend it.

It's the first time I've ever written an interpreter. There might be lots of errors in this project.
