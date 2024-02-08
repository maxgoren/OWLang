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
    "   func add(va,vb) begin",
    "       let vm := (va + vb);",
    "       print (vm);",
    "   end;",
    "   add(3,5);",
    "   add(1,2);",
    "   add(11,9);",
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


vector<string> loopAndFuncEx = {
        "program begin",
        "   let count := 1;",
        "   func helloworld(varB) begin",
        "         print (varB);",
        "   end;",
        "   while (count < 5) begin",
        "       helloworld(count);",
        "       count := (count + 1);",
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
            "    print (curr);",
            "    count := (count + 1);",
            "    next := (curr + prev);",
            "    prev := (curr);",
            "    curr := (next);",
            "end;",
        "end" 
};


int main(int argc, char* argv[]) {
        OwlCompiler omc;
        OwlMachine ovm;
        omc.compile(ex2, "fib.owlsm", true);
        ovm.loadProgramFromFile("fib.owlsm");
        ovm.start(true);
        cout<<"------------------------"<<endl;
    return 0;
}

