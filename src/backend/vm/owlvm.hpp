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
        bool didAccess;
        int ma;
        void load_constant();
        void load_from_addr();
        void store_to_addr();
        void mark_stack();
        void call_proc();
        void enter_proc();
        void ret_proc();
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

void OwlVM::printStack() {
    cout<<"[ op:["<<instAsStr[currentInstruction.op]<<" "<<currentInstruction.addr<<" "<<currentInstruction.val<<"], IP: "<<IP<<", BP: "<<BP<<", SP: "<<sp<<", EP: "<<ep<<" SL: "<<stack_level<<" ";
    cout<<"] [ ";
    for (int i = 0; i < sp + 10; i++) {
        if (i > sp+1 && dstack[i] == 0 && dstack[i+1] == 0 && dstack[i+2] == 0 && dstack[i+3] == 0 &&dstack[i+4] == 0) {
            break;
        }
        if (i == BP) cout<<">|";
        if (i == sp)              cout<<"{";
        if (didAccess && i == ma) cout<<"(";
        cout<<dstack[i];
        if (didAccess && i == ma) cout<<")";
        if (i == sp)              cout<<"}";
        cout<<" ";
    }
    cout<<"]"<<endl;
    if (didAccess) didAccess = false;
}

void OwlVM::init() {
    stack_level = 1;
    BP = 1;
    sp = BP+st.size();
    cout<<sp<<endl;
    IP = 0;
    ep = sp;
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
    init();
    cout<<"[OwlVM v0.2 Starting]\n";
    do {
        fetchInstruction();
        if (trace == VERBOSE) printRegisters();
        executeInstruction();
        if (trace != OFF) printStack();
    } while (currentInstruction.op != HLT);
    cout<<"[OwlVM Done.]"<<endl;
}

void OwlVM::print() {
     cout<<dstack[sp]<<endl; 
     sp--;
}


int OwlVM::findLocal(int nst) {
    int bl = BP;
    while (nst > 1) {
        bl = dstack[BP];
        nst--;
    }
    return stack_level == 1 ? bl:bl+5;
}

void OwlVM::mark_stack() {
   
}

void OwlVM::ret_proc() {
    sp = BP-1;            //reset stack ptr
    //sp = dstack[BP+2];  
    IP = dstack[BP+1]; //pop return address off stack
    BP = dstack[BP]; //reset frameptr;
    dstack[sp] = 0;
    stack_level--;
}

void OwlVM::call_proc() {
    dstack[sp+2] = BP; 
    dstack[sp+3] = IP;   
    dstack[sp+4] = sp;  
    BP = sp+2;
    sp = BP+5;
    ep = sp;
    stack_level++;
    IP = currentInstruction.addr+1; //MAKE JUMP TO PROCEDURE
}

void OwlVM::enter_proc() {
    //move stack pointer ahead of local params
    sp += currentInstruction.val+1;
}

void OwlVM::load_constant() {
    sp++;
    dstack[sp] = currentInstruction.val;  
    if (sp > ep) ep = sp;
}

void OwlVM::load_from_addr() {
    sp++; 
    ma = stack_level > 1 ? (BP-(currentInstruction.addr+1)):(findLocal(stack_level) + currentInstruction.addr);
    dstack[sp] = dstack[ma]; 
    if (sp > ep) ep = sp;
    didAccess = true; 
}

void OwlVM::store_to_addr() {
    ma = stack_level > 1 ? (BP-(currentInstruction.addr+1)):findLocal(stack_level) + currentInstruction.addr;
    dstack[ma] = dstack[sp];
    didAccess = true;
    sp--;
}

void OwlVM::executeInstruction() {
    bool should_inc = false;
    switch (currentInstruction.op) {
    case LDC: //LOAD CONSTANT 
        load_constant();
        break;
    case LOD: //LOAD FROM ADDRESS
        load_from_addr();
        break;
    case STO:
        store_to_addr();
        break;
    case MST: //MARK STACK
        mark_stack();
        break;
    case CALL: //CALL PROCEDURE 
        call_proc();
        break;
    case ENT: //ENTER PROCEDURE 
        enter_proc();
        break;
    case RET: //return from procedure
        ret_proc();
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
        break;
    case NOP:  
        nopcount++;
        if (nopcount > 50)
            IP = 0;
        break;
    case PRI: 
        print(); 
        break;
    case HLT: 
        IP = 0; 
        break;
    default:
        break;
    }
}

#endif