#include <iostream>
#include <vector>
#include "../src/owlCompiler.hpp"
using namespace std;

vector<string> ex1 = {
        "program begin", 
            "let myVar := (15 + 5);",
            "let myOtherVar := (20 + (30  + 1));",
            "print myVar;",
            "print myOtherVar;",
            "let varb := (myVar + myOtherVar);",
            "print varb;"
        "end" 
};

vector<string> ex2 = {
        "program begin",
        "   let counter := 1;",
        "   while (counter < 5) begin",
        "       print counter;",
        "       let counter := (counter + 1);"
        "   end",
        "end"
};

vector<string> ex3 = {
    "program begin",
    "   if (3 < 6) then",
    "       print 42;",
    "   else",
    "       print 16;",
    "       print 33;",
    "   end;",
    "   if (7 < 3) then",
    "       print 42;",
    "   else",
    "       print 13;",
    "   end;",
    "end"
};

vector<string> fibEx = {
        "program begin", 
            "let count := 0;",
            "let prev := 0;",
            "let curr := 1;",
            "let next := 0;",
            "while (count < 10) begin",
            "    print curr;",
            "    let count := (count + 1);",
            "    let next := (curr + prev);",
            "    let prev := curr;",
            "    let curr := next;",
            "end;",
        "end" 
};

int main(int argc, char* argv[]) {
    OwlCompiler omc;
    OwlMachine ovm;
    omc.compile(ex3, "fib.owlsm", true);
    ovm.loadProgram("fib.owlsm");
    ovm.start(false);
    return 0;
}

