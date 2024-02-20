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
    IXA, //Indexed Address
    IDL, //Indirect Load
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
    "LBL", "LDC", "LDA", "LOD", "IXA", "IDL", "STO", 
    "MST", "CAL", "ENT", "RET",
    "JMP", "JEQ", "JNE", 
    "ADI", "SBI", "MPI", "DVI", "NEG",
    "EQU", "NEQ", "LT", "LTE", "GT", "GTE", 
    "NOP", "PRI", "HLT"
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
        label = " ";
    }
    void output(ostream& os) {
        os << instAsStr[op] << ' ' << addr << ' ' << val << ' ' << label << "\r\n";
    }
};

#endif