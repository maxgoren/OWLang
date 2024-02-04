# OWLang
OWLang - The "One Weekend" Programming Language Project.

## What is it?
This is the code repository for the "one weekend" language (owlang) compiler project that I am developing.
owlang is a toy programming language, meant to teach the fundamentals of programming
language design, with a focus on compiler implementation, in a relaxed fun way that can be accomplished
by a someone familiar with programming, but not necessarily compilers, in "one weekend"(YTMV).

The actual "lessons" are still a work in progress, but the "curriculum" is developed to be covered in the course of
*drum roll* one weekend! Starting on Friday evening and wrapping up on sunday and can be followed as such: 

 ## Friday 
 ### Evening:
Establishing what the language should look like
and its key words, "general syntax".
String searching algorithms and Regular 
Expressions and their relationship to compilers.

#### By the end of Friday night:
The scanner/Lexer will have been completed, providing a useable tokenized representation
of our input language to be used by the parser.

## Saturday 
### Morning:
More about syntax
EBNF Grammar
### Afternoon:
Recursive Descent Parsing
### Evening:
Abstract Syntax Trees.

#### By the end of Saturday night:
  The parser will be complete, outputting an abstract syntax tree suitable for use
  with a tree walking interpreter, or for use in generating intermediary code.

## Sunday 
### Morning:
 - A discussion on interpreters vs compilers.
 - Runtime Environments
 - Stack Machines & P-Code
### Afternoon:
 - Stack Machines contd.
 - Type checking (optional)
 - Code Generation
 
#### By Sunday evening:
you will have a working compiler for owlang that creates
p-code that is executed on the OWLVM, a stack machine.
