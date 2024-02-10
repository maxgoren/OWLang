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
#ifndef owlvm_ds_hpp
#define owlvm_ds_hpp
#include <iostream>
#include <vector>
using namespace std;

enum vmop {
    LBL, //lbl
    LDC, //LOAD CONSTANT 
    LDA, //LOAD ADDRESS 
    LOD, //LOAD FROM ADDRESS
    STO, //STORE TO ADDRESS
    MST, //MARK STACK 
    CALL, //CALL PROCEDURE 
    ENT, //ENTER PROCEDURE 
    RET, //return from procedure
    JMP, //unconditional jump 
    JEQ, //jump if true 
    JNE, //jump if false
    ADI, SBI, MPI, DVI, //integer math
    NEG,
    EQU, NEQ, LT, LTE, GT, GTE, //relops 
    NOP,
    PRI,
    HLT
};

vector<string> instAsStr = {
    "LBL", //label
    "LDC", //LOAD CONSTANT 
    "LDA", //LOAD ADDRESS 
    "LOD", //LOAD FROM ADDRESS
    "STO", //STORE TO ADDRESS
    "MST", //MARK STACK 
    "CALL", //CALL PROCEDURE 
    "ENT", //ENTER PROCEDURE 
    "RET", //return from procedure
    "JMP", //unconditional jump 
    "JEQ", //jump if true 
    "JNE", //jump if false
    "ADI", "SBI", "MPI", "DVI", //integer math
    "NEG",
    "EQU", "NEQ", "LT", "LTE", "GT", "GTE", //relops 
    "NOP",
    "PRI",
    "HLT"
};

struct Instruction {
    vmop op;
    int addr;
    int val;
    string label;
    Instruction(vmop mnemonic = NOP, int address = 0, int value = 0) {
        op = mnemonic;
        addr = address;
        val = value;
        label = "";
    }
    void output(ostream& os) {
        os << instAsStr[op] << ' ' << addr << ' ' << val << ' ' << label << "\r\n";
    }
};

#endif
