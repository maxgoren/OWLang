# OWLang
OWLang - The "One Weekend" Programming Language Project.

*Primary Development of Owl has moved to https://github.com/maxgoren/OwlInterpreter

## What is OWLang?
Besides a hoot of a good time? Owlang is a toy programming language developed for exploring the fundamentals 
of programming language design with a focus on compiler implementation. The "One Weekend" programming language project
is intended to help learn the fundamentals of implementing a compiler for a programming language in a relaxed, fun way 
that can be accomplished by a someone familiar with programming, but not necessarily compilers, in "one weekend"(YTMV).

Despite begin a "toy" language, Owl is surprisingly featured, including control flow using if/then statements, while loops,
procedures(with recursion!), and arrays.

## Ok, So wheres the "One Weekend" Thing?
The actual "lessons" are still a work in progress, but the "curriculum" is developed to be covered 
in the course of *drum roll* one weekend! 

If you have a weekend to kill, and want to learn a bit more about how compilers turn source code into executable programs,
You can spend a weekend doing the following: 

 ## Friday 
 ### Evening:
 - Establishing what the language should look like, its key words, and a general syntax.
 - String searching algorithms, Regular Expressions and their relationship to compilers.
 - Lexical analysis/tokenization
#### our toy language should allow us to do stuff like this
```
    program begin 
        let myVar := (20 + (30  + 1));
       while (myVar >= 1) do
          print myVar;
          myVar := myVar / 3;
       end
       if (myVar == 1) begin
         print 1;
       else
         print 2;
      end
    end
```
You'd be surprised how much you can do with just looping, infix integer math, assignment, and if statements.
If you have a _long_ weekend at your disposalm, Procedures, arrays, and structs are fun additions.

#### By the end of Friday night:
 - The lexer will have been completed providing a useable tokenized representation
of our input language to be used by the parser we will build.

```
    0:   <BEGIN, begin>
    1:   <LET, let>
    1:   <ID, myVar>
    1:   <ASSIGN, :=>
    1:   <LPAREN, (>
    1:   <NUM, 20>
    1:   <PLUS, +>
    1:   <LPAREN, (>
    1:   <NUM, 30>
    1:   <PLUS, +>
    1:   <NUM, 1>
    1:   <RPAREN, )>
    1:   <RPAREN, )>
    1:   <SEMI, ;>   
    2:   <PRINT, print>
    2:   <ID, myVar>
    2:   <SEMI, ;>
    3:   <END, end>
```

## Saturday 
### Morning:
 - More about syntax
 - EBNF Grammar
### Afternoon:
 - Recursive Descent Parsing
 - Parse Trees
### Evening:
- Abstract Syntax Trees.
- Different uses of AST's
- a quick aside on tree-walking interpreters

#### By the end of Saturday night:
  The parser will be complete, outputting an abstract syntax tree suitable for use
  with a tree walking interpreter, or for use in generating intermediary code.

```
    [ASSIGNSTM] myOtherVar, ,
    [OP_EXPR] , , PLUS
      [CONST_EXPR] , 20,
      [OP_EXPR] , , PLUS
        [CONST_EXPR] , 30,
        [CONST_EXPR] , 1,
  [PRINTSTM] , ,
    [ID_EXPR] myOtherVar, ,
```

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
- you will have a working compiler for owlang that outputs
p-code that is executed on the OWLVM, a stack machine.

The code generator can output p-code with or without annotation
```
<- push 20
lit 0, 20
<- push 30
lit 0, 30
<- push 1
lit 0, 1
<- add
opr 0, 2
<- add
opr 0, 2
<- save top of stack to mem address 0
sto 0, 0
<- push contents of mem address 0 to top of stack
lod 0, 0
pri
```

same code as above, without optional annotations
```
lit 0, 20
lit 0, 30
lit 0, 1
opr 0, 2
opr 0, 2
sto 0, 0
lod 0, 0
pri
```

```
#include <iostream>
#include <vector>
#include "owlCompiler.hpp"
using namespace std;


int main() {
    vector<string> owlCode = {
        "begin", 
            "let myVar := (15 + 5);",
            "let myOtherVar := (20 + (30  + 1));",
            "print myVar;",
            "print myOtherVar;",
            "let varb := (myVar + myOtherVar);",
            "print varb;"
        "end" 
    };
    OwlCompiler omc;
    omc.compile(owlCode, false);
}
max@MaxGorenLaptop:/mnt/c/Users/mgoren/Desktop/codeEx/owlCompiler$ g++ owlEx.cpp -o owlEx
max@MaxGorenLaptop:/mnt/c/Users/mgoren/Desktop/codeEx/owlCompiler$ ./owlEx
[OWLVM v0.1 Starting...]
20
51
71
[OWLVM Done.]
max@MaxGorenLaptop:/mnt/c/Users/mgoren/Desktop/codeEx/owlCompiler$
```

You can choose to output which instruction the VM is working on, or just run it normally
```
[OWLVM v0.1 Starting...]
[0]0 0, 20
[1]0 0, 30
[2]0 0, 1
[3]1 0, 2
[4]1 0, 2
[5]3 0, 0
[6]2 0, 0
[7]8 0, 0
51
[8]9 0, 0
[OWLVM Done.]

[OWLVM v0.1 Starting...]
51
[OWLVM Done.]
```
