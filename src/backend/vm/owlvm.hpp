#ifndef owlvm_hpp
#define owlvm_hpp
#include <iostream>
#include <vector>
#include "owlvm_ds.hpp"
using namespace std;
const int MAXSTACK = 2500;
const int MAXCODE = 1000;

class OwlVM {
    private:
        typedef int REGISTER;
        Instruction codePage[MAXCODE];
        int dstack[MAXSTACK];
        REGISTER sp;  //stack pointer
        REGISTER fp; //frame pointer
        REGISTER ip; //instruction pointerf
        REGISTER lp; //local variable pointer
        void init();
        Instruction currentInstruction;
        void fetchInstruction();
        void executeInstruction();
        void print();
        void printStack();
        int findBase(int l);
    public:
        OwlVM();
        void start();
        void loadProgram(vector<Instruction> code);
};

OwlVM::OwlVM() {
    init();
}

void OwlVM::init() {
    sp = 0;
    fp = 1;
    ip = 0;
    lp = 0;
    for (int i = 0; i < MAXSTACK; i++) {
        dstack[i] = 0;
    }
}

void OwlVM::fetchInstruction() {
    currentInstruction = codePage[ip++];
}

void OwlVM::loadProgram(vector<Instruction> code) {
    for (int i = 0; i < code.size(); i++) {
        codePage[i] = code[i];
    }
}

void OwlVM::start() {
    cout<<"[OwlVM v0.2 Starting]\n";
    do {
        fetchInstruction();
        currentInstruction.output(cout);
        executeInstruction();
        printStack();
    } while (ip != 0);
    cout<<"[OwlVM Done.]"<<endl;
}

void OwlVM::print() {
     cout<<dstack[sp]<<endl; 
     sp--; 
}

void OwlVM::printStack() {
    cout<<"[ op:"<<instAsStr[currentInstruction.op]<<", ip: "<<ip<<", sp: "<<sp<<", fp: "<<fp;
    cout<<"] [ ";
    for (int i = 0; i < sp + 10; i++) {
        if (i > sp && dstack[i] == 0 && dstack[i+1] == 0 && dstack[i+2] == 0 && dstack[i+3] == 0 &&dstack[i+4] == 0) {
            break;
        }
        if (i == fp) cout<<"| ";
        cout<<dstack[i]<<" ";
    }
    cout<<"]"<<endl;
}

int OwlVM::findBase(int l) {
    int nb = fp;
    while (l > 0) {
        nb = dstack[nb]; 
        l--;
    }
    return nb+lp;
}

void OwlVM::executeInstruction() {
    bool should_inc = false;
    switch (currentInstruction.op) {
    case LDC: //LOAD CONSTANT 
        sp++;
        dstack[sp] = currentInstruction.val;  
        break;
    case LOD: //LOAD FROM ADDRESS
        sp++; 
        dstack[sp] = dstack[findBase(0) + currentInstruction.addr]; 
        break;
    case STO:
        if (findBase(0)+currentInstruction.addr == 0) should_inc = true;
        dstack[findBase(0) + currentInstruction.addr] = dstack[sp];
        if (should_inc) lp++;
        sp--;
        break;
    case MST: //MARK STACK
        dstack[sp+1] = 0;
        dstack[sp+2] = fp;
        dstack[sp+3] = ip; //push the return address to stack;
        fp = sp+1;
        ip++;
        break;
    case CALL: //CALL PROCEDURE 
        ip = currentInstruction.addr + 1;
        break;
    case ENT: //ENTER PROCEDURE 
        //move stack pointer ahead of local params
        lp = currentInstruction.val;
        sp = fp + lp;
        break;
    case RET: //return from procedure
        sp = fp;        //reset stack ptr
        ip = dstack[sp+3]; //pop return address off stack
        fp = dstack[sp+2]; //set frameptr;
        break;
    case JMP:  //unconditional jump 
        ip = currentInstruction.addr;
        break;
    case JEQ: //jump if true 
        if (dstack[sp] == 1) {
            ip = currentInstruction.addr;
        }
        sp--;
        break;
    case JNE: //jump if false
        if (dstack[sp] == 0) {
            ip = currentInstruction.addr;
        }
        sp--;
        break;
                //integer math
    case ADI: 
        sp--; 
        dstack[sp] = dstack[sp] + dstack[sp+1];
        break;
    case SBI: 
        sp--; 
        dstack[sp] = dstack[sp] - dstack[sp+1];
        break;
    case MPI: 
        sp--; 
        dstack[sp] = dstack[sp] * dstack[sp+1];
        break;
    case DVI:
        sp--; 
        dstack[sp] = dstack[sp] + dstack[sp+1];
        break; 
    case NEG:
        dstack[sp] = -(dstack[sp]);
        break;
    case EQU: 
        sp--; 
        dstack[sp] = (dstack[sp] == dstack[sp+1]);
        break; 
    case NEQ: 
        sp--; 
        dstack[sp] = (dstack[sp] != dstack[sp+1]);
        break; 
    case LT: 
        sp--; 
        dstack[sp] = (dstack[sp] < dstack[sp+1]);
        break; 
    case LTE: 
        sp--; 
        dstack[sp] = (dstack[sp] <= dstack[sp+1]);
        break; 
    case GT: 
        sp--; 
        dstack[sp] = (dstack[sp] > dstack[sp+1]);
        break; 
    case GTE: //relops 
        sp--; 
        dstack[sp] = (dstack[sp] >= dstack[sp+1]);
        break; 
    case LBL:
    case NOP: 
        ip++; 
        break;
    case PRI: 
        print(); 
        break;
    case HLT: ip = 0; 
        break;
    default:
        break;
    }
}

#endif
