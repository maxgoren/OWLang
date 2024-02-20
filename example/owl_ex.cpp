#include <iostream>
#include "../src/owlc.hpp"
#include "../src/backend/vm/owlvm.hpp"
using namespace std;


vector<string> ifEx = {
    "program 'ifEx';",
    "begin",
    "   let x: int;",
    "   let y: int;",
    "   x := 7;",
    "   y := 12;",
    "   if (x < y) then",
    "       print (x+y);",
    "   end; ",
    "end"
};


vector<string> loopEx = {
    "program 'loopEx';",
    "begin",
    "   let x: int;",
    "   x := (1);",
    "   while (x < 6) begin",
    "       print (x);",
    "       x := (x + 1);",
    "   end;",
    "end"
};

vector<string> noParamProcEx = {
    "program 'noparamex';",
    "begin",
    "   let t: int;",
    "   t := (1);",
    "   func myProc() begin",
    "       print (t);",
    "       t := (t + 1);",
    "   end;",
    "   while (t < 10) begin",
    "       myProc();",
    "   end;"
    "end"
};

vector<string> loop2Ex = {
    "program 'loopTwoEx';",
    "begin",
    "   let x: int;",
    "   let y: int;",
    "   let j: int;",
    "   x := 1;",
    "   y := 2;",
    "   while (y < 20) begin",
    "       j := (x + y);",
    "       print j;",
    "       x := (x + 1);",
    "       y := (y + 1);",
    "   end;",
    "end"
};

vector<string> procEx = {
    "program 'proxEx';",
    "begin",
    "   let t: int;",
    "   let m: int;",
    "   let q: int;",
    "   func myFuncR(vx, vy) begin",
    "       q := (vx + vy);",
    "       print (q);",
    "   end;",
    "   t := (7);",
    "   m := (4);",
    "   myFuncR(t, m);",
    "end;"
};

vector<string> procLoopEx = {
    "program procLoopEx;",
    "begin",
    "   let p: int;",
    "   let t: int;",
    "   func myFunc(vx, vy) begin",
    "       p := (vx+vy);",
    "       print (p);",
    "   end;",
    "   t := (1);",
    "   while (t < 5) begin",
    "       myFunc(t, t+1);",
    "       t := (t+1);",
    "   end;",
    "end;"
};

vector<string> recursionEx = {
    "program 'recursionEx';",
    "begin",
    "   let p: int;",
    "   let c: int;",
    "   let n: int;",
    "   let m: int;",
    "   p := 0;",
    "   c := 1;",
    "   n := 0;",
    "   m := 1;",
    "   while (n < 10) begin",
    "       m := (c + p);",
    "       p := c;",
    "       c := m;",
    "       n := (n + 1);",
    "       print m;",
    "   end;",
    "end"
};

vector<string> fibEx = {
    "program fibEx;",
    "begin",
    "   let prev: int;",
    "   let curr: int;",
    "   let next: int;",
    "   let count: int;",
    "   prev := (0);",
    "   curr := (1);",
    "   next := (0);",
    "   count := (1);",
    "   while (curr < 150) begin",
    "       count := (count + 1);",
    "       next := (curr + prev);",
    "       print (next);",
    "       prev := curr;",
    "       curr := next;",
    "   end;",
    "end"
};

vector<string> factorial = {
    "program 'factorial';",
    "begin"
    "   let x: int;",
    "   let fact: int;",
    "   x := 5;",
    "   fact := 1;",
    "   while (0 < x) begin",
    "       fact := (fact * x);",
    "       x := (x - 1);",
    "   end;",
    "   print (fact);",
    "end"
};

vector<string> arrayEx = {
    "program 'arrayEx';",
    "begin",
    "   let m[10]: array;",
    "   let c: int;",
    "   c := 1;",
    "   while (c < 6) begin",
    "       m[c] := (42 + c);",
    "       c := (c+1);",
    "   end;",
    "   c := 1;",
    "   while (c < 6) begin",
    "       print (m[c]);",
    "       c := (c + 1);",
    "   end;",
    "end"
};


vector<vector<string>> programList = {
        ifEx, //0
        loopEx, //1
        loop2Ex,//2
        procEx, //3
        noParamProcEx, //4
        procLoopEx, //5
        recursionEx, //6
        fibEx,        //7
        factorial, //8
        arrayEx //9
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