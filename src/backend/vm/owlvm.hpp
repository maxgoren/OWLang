#ifndef owlvm_hpp
#define owlvm_hpp
#include <iostream>
#include <vector>
#include "owlvm_ds.hpp"
using namespace std;
const int MAXSTACK = 2500;
const int MAXCODE = 1000;
enum TraceLevel {
    OFF, ON, VERBOSE
};

class OwlVM {
    private:
        typedef int REGISTER;
        Instruction codePage[MAXCODE];
        int dstack[MAXSTACK];
        int stack_level;
        REGISTER sp;  //stack pointer
        REGISTER BP; //frame pointer
        REGISTER IP; //instruction pointerf
       // REGISTER lp; //local variable pointer
        REGISTER ep; //extreme pointer
        void init();
        Instruction currentInstruction;
        void fetchInstruction();
        void executeInstruction();
        void print();
        void printStack();
        void printRegisters();
        int findParam(int n);
        int findLocal(int n);
        int nopcount;
    public:
        OwlVM();
        void start(TraceLevel tace = OFF);
        void loadProgram(vector<Instruction> code);
};

OwlVM::OwlVM() {
    init();
}

void OwlVM::printRegisters() {
    cout<<"Instruction: "<<instAsStr[currentInstruction.op]<<" "<<currentInstruction.addr<<" "<<currentInstruction.val<<endl;
    cout<<"stack_level: "<<stack_level<<endl;
    cout<<"BP: "<<BP<<endl;
    cout<<"IP: "<<IP<<endl;
    cout<<"SP: "<<sp<<endl;
    cout<<"EP: "<<ep<<endl;
}

void OwlVM::init() {
    stack_level = 1;
    BP = 1;
    sp = BP+2;
    IP = 0;
    ep =sp;
    nopcount = 0;
    for (int i = 0; i < MAXSTACK; i++) {
        dstack[i] = 0;
    }
}

void OwlVM::fetchInstruction() {
    currentInstruction = codePage[IP++];
    if (ep >= MAXSTACK - 30){
        cout<<"[OUT OF STACK SPACE]"<<endl;
        exit(0);
    }
}

void OwlVM::loadProgram(vector<Instruction> code) {
    for (int i = 0; i < code.size(); i++) {
        codePage[i] = code[i];
    }
}

void OwlVM::start(TraceLevel trace) {
    cout<<"[OwlVM v0.2 Starting]\n";
    printStack();
    do {
        fetchInstruction();
        //printRegisters();
        executeInstruction();
        if (trace != OFF)
            printStack();
    } while (currentInstruction.op != HLT);
    cout<<"[OwlVM Done.]"<<endl;
}

void OwlVM::print() {
     cout<<dstack[sp]<<endl; 
     sp--; 
}

void OwlVM::printStack() {
    cout<<"[ op:["<<instAsStr[currentInstruction.op]<<" "<<currentInstruction.addr<<" "<<currentInstruction.val<<"], IP: "<<IP<<", BP: "<<BP<<", SP: "<<sp<<", EP: "<<ep<<" SL: "<<stack_level<<" ";
    cout<<"] [ ";
    for (int i = 0; i < sp + 10; i++) {
        if (i > sp+1 && dstack[i] == 0 && dstack[i+1] == 0 && dstack[i+2] == 0 && dstack[i+3] == 0 &&dstack[i+4] == 0) {
            break;
        }
        if (i == BP) cout<<">|";
        if (i == sp) cout<<"{";
        cout<<dstack[i];
        if (i == sp) cout<<"} ";
        else cout<<" ";
    }
    cout<<"]"<<endl;
}


int OwlVM::findLocal(int nst) {
    int bl = BP;
    while (nst > max(stack_level - 1, 0)) {
        bl = dstack[bl];
        nst--;
    }
    return bl + 4;
}

void OwlVM::executeInstruction() {
    bool should_inc = false;
    switch (currentInstruction.op) {
    case LDC: //LOAD CONSTANT 
        sp++;
        dstack[sp] = currentInstruction.val;  
        if (sp > ep) ep = sp;
        break;
    case LOD: //LOAD FROM ADDRESS
        sp++; 
        dstack[sp] = dstack[findLocal(stack_level) + currentInstruction.addr]; 
        if (sp > ep) ep = sp;
        break;
    case STO:
        dstack[findLocal(stack_level) + currentInstruction.addr] = dstack[sp];
        if ((findLocal(stack_level) + currentInstruction.addr) > ep) ep = sp;
        //sp--;
        break;
    case MST: //MARK STACK
        dstack[ep+1] = 0;    //SAVE POSITION FOR RETURN VALUE
        dstack[ep+2] = BP+1;   //PUSH FRAME POINTER;
        dstack[ep+3] = 0;   //sAVE SPOT FOR RETURN ADDRESS;
        dstack[ep+4] = ep;  //PUSH extreme pointer on stack
        BP = ep+2;
        sp = BP + 3;
        ep = sp;
        stack_level++;
        break;
    case CALL: //CALL PROCEDURE 
        dstack[BP+2] = IP; //PUSH RETURN ADDRESS ON TO STACK
        IP = currentInstruction.addr+1; //MAKE JUMP TO PROCEDURE
        break;
    case ENT: //ENTER PROCEDURE 
        //move stack pointer ahead of local params
        sp += currentInstruction.val-1;
        break;
    case RET: //return from procedure
        sp = BP;            //reset stack ptr
        ep = dstack[BP+3];  //reset extreme ptr
        IP = dstack[BP+2]; //pop return address off stack
        BP = dstack[findLocal(stack_level) + BP+1]; //reset frameptr;
        dstack[sp] = 0;
        stack_level--;
        break;
    case JMP:  //unconditional jump 
        IP = currentInstruction.addr;
        break;
    case JEQ: //jump if true 
        if (dstack[sp] == 1) {
            IP = currentInstruction.addr;
        }
        sp--;
        break;
    case JNE: //jump if false
        if (dstack[sp] == 0) {
            IP = currentInstruction.addr;
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
        nopcount++;
        if (nopcount == 3)
            IP = 0;
        break;
    case PRI: 
        print(); 
        break;
    case HLT: IP = 0; 
        break;
    default:
        break;
    }
}

#endif