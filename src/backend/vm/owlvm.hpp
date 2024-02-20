#ifndef owlvm_hpp
#define owlvm_hpp
#include <iostream>
#include <iomanip>
#include <vector>
#include "../../frontend/lexer.hpp"
#include "../../frontend/parser.hpp"
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
        int getFromAddress(int address);
        int nopcount;
        bool didAccess;
        int ma;
        void load_constant();
        void load_addr();
        void load_from_addr();
        void indexed_address();
        void indirect_load();
        void store_to_addr();
        void mark_stack();
        void call_proc();
        void enter_proc();
        void ret_proc();
    public:
        OwlVM();
        void start(TraceLevel tace = OFF);
        void loadProgram(vector<Instruction> code);
        void repl();
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
    cout<<"[ op:["<<setw(3)<<instAsStr[currentInstruction.op]<<" "<<setw(4)<<currentInstruction.addr<<" "<<setw(3)<<currentInstruction.val<<"], IP: "<<setw(3)<<IP<<", BP: "<<setw(3)<<BP<<", SP: "<<setw(3)<<sp<<", MA: "<<setw(3)<<ma<<" SL: "<<setw(3)<<stack_level<<" ";
    cout<<"] [ ";
    for (int i = 0; i < sp + 10; i++) {
        if (i > sp+1 && dstack[i] == 0 && dstack[i+1] == 0 && dstack[i+2] == 0 && dstack[i+3] == 0 &&dstack[i+4] == 0) {
            break;
        }
        if (i == BP) cout<<">|";
        if (i == sp)              cout<<"{";
        if (didAccess && i == ma) cout<<"(";
        int putin = dstack[i];
        cout<<putin;
        if (didAccess && i == ma) cout<<")";
        if (i == sp)              cout<<"}";
        cout<<" ";
    }
    cout<<"]"<<endl;
    cout<<"[Memory: ";
    for (int i = 2475; i < 2500; i++) {
        cout<<dstack[i]<<" ";
    }
    cout<<"]"<<endl;
    if (didAccess) didAccess = false;
}

void OwlVM::init() {
    stack_level = 1;
    BP = 1;
    sp = BP+1;
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

void OwlVM::repl() {
    OwlLexer lex;
    OwlParser parser;
    init();
    cout<<"[OwlVM v0.2 Starting REPL]\n";
    int i = 0;
    string input;
    do {
        cout<<"["<<++i<<"]owl> ";
        getline(cin, input);
    } while (currentInstruction.op != HLT);
    cout<<"[OwlVM Done.]"<<endl;
    for (int i = 2450; i < 2500; i++)
        cout<<dstack[i]<<" ";
    cout<<endl;
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
       for (int i = 2450; i < 2500; i++)
        cout<<dstack[i]<<" ";
    cout<<endl;
}

void OwlVM::print() {
     cout<<dstack[sp]<<endl; 
     sp--;
}


int OwlVM::getFromAddress(int address) {
    if (address > 2000)
        return address;
    return stack_level > 1 ? (BP-(address+1)):(dstack[BP] + address);
}

void OwlVM::mark_stack() {

}

void OwlVM::ret_proc() {
    int ra = sp;
    sp = BP-1;            //reset stack ptr
    IP = dstack[BP+1]; //pop return address off stack
    BP = dstack[BP]; //reset frameptr;
    dstack[sp] = dstack[ra];
    stack_level--;
}

void OwlVM::call_proc() {
    dstack[sp+2] = BP; 
    dstack[sp+3] = IP;   
    dstack[sp+4] = sp;  
    BP = sp+2;
    sp = BP+3;
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
    ma = sp;
    if (sp > ep) ep = sp;
}

void OwlVM::load_from_addr() {
    sp++; 
    ma = getFromAddress(currentInstruction.addr);
    dstack[sp] = dstack[ma]; 
    if (sp > ep) ep = sp;
    didAccess = true; 
}

void OwlVM::load_addr() {
    sp++;
    ma = sp;
    dstack[sp] = getFromAddress(currentInstruction.addr);
}

void OwlVM::store_to_addr() {
    ma = getFromAddress(dstack[sp-1]);
    dstack[ma] = dstack[sp];
    didAccess = true;
    sp--;
}

void OwlVM::indexed_address() {
    int offset = dstack[sp];
    int badr = dstack[sp-1];
    sp-=2;
    ma = sp;
    dstack[ma] = badr - offset;
    didAccess = false;
}

void OwlVM::indirect_load() {
    sp++;
    ma = getFromAddress(dstack[sp-1] - currentInstruction.addr);
    dstack[sp] = dstack[ma];
    didAccess = true;
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
    case LDA: //LOAD ADDRESS
        load_addr();
        break;
    case STO: //STORE TO ADDRESS
        store_to_addr();
        break;
    case IXA:
        indexed_address();
        break;
    case IDL:
        indirect_load();
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
        //cout<<dstack[sp]<<" + "<<dstack[sp+1]<<endl;
        dstack[sp] = dstack[sp] + dstack[sp+1];
        break;
    case SBI: 
        sp--; 
        //cout<<dstack[sp]<<" - "<<dstack[sp+1]<<endl;
        dstack[sp] = dstack[sp] - dstack[sp+1];
        break;
    case MPI: 
        sp--; 
        //cout<<dstack[sp]<<" * "<<dstack[sp+1]<<endl;
        dstack[sp] = dstack[sp] * dstack[sp+1];
        break;
    case DVI:
        sp--; 
        //cout<<dstack[sp]<<" / "<<dstack[sp+1]<<endl;
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
        //cout<<dstack[sp]<<" < "<<dstack[sp+1]<<endl;
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