#include <iostream>
#include "../src/owlc.hpp"
#include "../src/backend/vm/owlvm.hpp"
using namespace std;


vector<string> ifEx = {
    "program 'ifEx';",
    "begin",
    "   let x := 7;",
    "   let y := 12;",
    "   if (x < y) then",
    "       print (x+y);",
    "   end; ",
    "end"
};


vector<string> loopEx = {
    "program 'loopEx';",
    "begin",
    "   int x := (1);",
    "   while (x < 6) begin",
    "       print (x);",
    "       x := (x + 1);",
    "   end;",
    "end"
};

vector<string> noParamProcEx = {
    "program 'noparamex';",
    "begin",
    "   int t := (1);",
    "   func myProc() begin",
    "       print (t);",
    "   end;",
    "   myProc();",
    "end"
};

vector<string> loop2Ex = {
    "program 'loopTwoEx';",
    "begin",
    "   int x := 1;",
    "   int y := 2;",
    "   while (y < 20) begin",
    "       let j := (x + y);",
    "       print j;",
    "       x := (x + 1);",
    "       y := (y + 1);",
    "   end;",
    "end"
};

vector<string> procEx = {
    "program 'proxEx';",
    "begin",
    "   func myFunc(vx, vy) begin",
    "       int q := (vx + vy);",
    "       print (q);",
    "       return q;",
    "   end;",
    "   int t := (1);",
    "   myFunc(t, (t+1));",
    "   print (42);"
    "end;"
};

vector<string> procLoopEx = {
    "program procLoopEx;",
    "begin",
    "   func myFunc(vx, vy) begin",
    "       print (vx+vy);",
    "   end;",
    "   int t := (1);",
    "   while (t < 5) begin",
    "       myFunc(t, t+1);",
    "       t := (t+1);",
    "   end;",
    "end;"
};

vector<string> recursionEx = {
    "program recursionEx;",
    "begin",
    "   func addOneAndPrint(vara) begin",
    "       int t := (vara + 1);",
    "       if (vara < 5) then",
    "           print (vara);",
    "           addOneAndPrint(t);",
    "       end;",
    "   end;",
    "   addOneAndPrint(1);",
    "   print (42);",
    "end"
};

vector<string> fibEx = {
    "program fibEx;",
    "begin",
    "   int prev := (0);",
    "   int curr := (1);",
    "   int next := (0);",
    "   int count := (1);",
    "   while (count < 10) begin",
    "       count := (count + 1);",
    "       next := (curr + prev);",
    "       print (next);",
    "       prev := curr;",
    "       curr := next;",
    "   end;",
    "end"
};



vector<vector<string>> programList = {
        ifEx, 
        loopEx, 
        loop2Ex,
        procEx, 
        noParamProcEx,
        procLoopEx,
        recursionEx,
        fibEx
        };

TraceLevel tl = OFF;

void compileAndRun(vector<string> pg) {
    OwlVM owlvm;
    OwlCompiler compiler;
    auto ast = compiler.compile(pg, "asm.owlsm", true); 
    owlvm.loadProgram(ast);
    owlvm.start(tl);
}


int main(int argc, char *argv[]) {
    if (argc < 2)
        tl = OFF;
    else {
         if (argv[1][0] == 'v') { 
            tl = VERBOSE;
         } else if (argv[1][0] == 'd') {
            tl = ON;
         } else {
            tl = OFF;
         }
    }
    compileAndRun(programList[atoi(argv[2])]);
}