/*

MIT License

Copyright (c) 2024 Max Goren, http://www.maxgcoding.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include <iostream>
#include "owlc.hpp"
#include "backend/vm/owlvm.hpp"
#include "backend/vm/owlvm_ds.hpp"
using namespace std;

vector<string> program = {
    "program begin",
    "   let x := 7;",
    "   let y := 12;",
    "   if (x < y) then",
    "       print (x+y);",
    "   end; ",
    "end"
};

vector<string> fibEx = {
    "program begin",
    "   let count := (0);",
    "   let prev := (0);",
    "   let curr := (1);",
    "   let next := (0);",
    "   print (prev);",
    "   print (curr);",
    "   while (count < 10) begin",
    "       count := (count + 1);"
    "       next := (curr + prev);",
    "       print (next);",
    "       prev := curr;",
    "       curr := next;",
    "   end;",
    "end"
};

vector<string> procEx = {
    "program begin",
    "   func addAndPrint(vara, varb) begin",
    "       print (vara + varb);",
    "   end;",
    "   let i := (1);",
    "   while (i < 7) begin",
    "       addAndPrint(i, (i+2));",
    "       i := (i + 1);",
    "   end;",
    "end"
};

int main() {
    OwlCompiler compiler; 
    vector<Instruction> owlAsm = compiler.compile(fibEx, "asm.owlsm", false);
    OwlVM owlvm;
    owlvm.loadProgram(owlAsm);
    owlvm.start(OFF);
}