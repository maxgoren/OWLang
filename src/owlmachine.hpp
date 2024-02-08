#ifndef owlmachine_hpp
#define owlmachine_hpp
#include <iostream>
#include <vector>
#include <fstream>
#include "owlmachine_ins.hpp"
using namespace std;

//lit 0, a load constant a
//opr 0, a execute operation a
//lod l, a load variable l, a
//sto l, a store variable l, a
//cal l, a call procedure a at level l
//inc 0, a increment t register by a
//jmp 0, a jump conditional
//jpc 0, a jump conditional to a
class OwlMachine {
    private:
        const int MAXADDR = 2047;
        const int MAXNEST = 3;
        vector<Instruction> codePage;
        bool loud;
        typedef int REGISTER;
        REGISTER sp;
        REGISTER bp;
        REGISTER pc;
        REGISTER ins;
        Instruction curr;
        static const int stackSize = 2600;
        int dstack[stackSize];
        int memStore[2600];
        int findBase(int level) {
            int b1 = bp;
            while (level > 0) {
                b1 = dstack[b1];
                level--;
            }
            return b1;
        }
        void init() {
            sp = 0; //<- stack pointer
            bp = 1; //<- frame pointer
            pc = 0;  //<- program counter
            for (int i = 0; i < stackSize; i++) 
                dstack[i] = 0;
        }
        void doReturn() {
            sp = bp - 1;
            pc = dstack[sp + 4];
            bp = dstack[sp + 3];
        }
        void doEnter() {
                dstack[sp + 1] = 0;
                dstack[sp + 2] = findBase(curr.level);
                dstack[sp + 3] = bp;
                dstack[sp + 4] = pc;
                bp = sp + 1;
                pc = curr.address;
        }
        void doOperation();
        void printStack() {
            cout<<"[pc: "<<pc<<"]"<<(curr.inst < ominststr.size() ? ominststr[curr.inst]:"LBL")<<" "<<curr.level<<" "<<curr.address<<" ";
            cout<<"Stack: [ ";
            int i = 0;
            for (;i <= stackSize; i++) {
                if (i > sp && dstack[i+1] == 0 && dstack[i+2] == 0 && dstack[i+3] == 0 && dstack[i+4] == 0) {
                    break;
                }
                if (i == bp)
                    cout<<"| ";
                cout<<dstack[i]<<" ";
            }
            cout<<"]"<<endl;
        }
        void fetchInstruction() {
            curr = codePage[pc];
            pc += 1;
        }
        void executeInstruction();
    public:
        OwlMachine() {
            init();
        }
        void start(bool trace);
        void loadProgram(vector<Instruction> program) {
            codePage = program;
        }
        void loadProgramFromFile(string filename);
};

void OwlMachine::start(bool trace) {
    loud = trace;
    int i = 0;
    cout<<code.size()<<endl;
    cout<<"[OWLVM v0.1 Starting...]"<<endl;
    do {
        fetchInstruction();
        executeInstruction();
        if (loud)
            printStack();
        
    } while (pc > 0 && pc < code.size());
    cout<<"[OWLVM Done.]"<<endl;
}

void OwlMachine::loadProgramFromFile(string filename) {
    vector<string> asmCode;
    ifstream infile;
    infile.open(filename);
    if (infile.is_open()) {
        string line;
        while (getline(infile, line))
            asmCode.push_back(line);
        infile.close();
    }
    for (string line : asmCode) {
        if (line.substr(0,2) == "<-") continue;
        string instr = line.substr(0, 3);
        string levstr = "";
        string adrstr = "";
        int i = 3;
        bool hitcoma = false;
        while (i < line.size()) {
            if (isdigit(line[i])) {
                string num;
                while (isdigit(line[i]))
                        num.push_back(line[i++]);
                if (line[i] == ',') hitcoma = true;
                if (hitcoma) adrstr = num;
                else levstr = num;
            }
            i++;
        }
        Instruction next(getominst(instr), atoi(levstr.c_str()), atoi(adrstr.c_str()));
        codePage.push_back(next);
    }
}


void OwlMachine::executeInstruction() {
    switch (curr.inst) {
            case lit: 
                sp += 1;
                dstack[sp] = curr.address;
                break;
            case opr:
                doOperation();
                break;
            case lod:
                sp += 1;
                dstack[sp] = dstack[findBase(curr.level) + curr.address];
                break;
            case sto:
                dstack[findBase(curr.level) + curr.address] = dstack[sp];
                break;
            case cal:
                doEnter();
                break;
            case inc:
                sp = sp + curr.address;
                break;
            case jmp:
                pc = curr.address;
                break;
            case jpc:
                if (dstack[sp] == 0) {
                    pc = curr.address;
                }
                sp--;
                break;
            case jeq:
                if (dstack[sp] == 1) {
                    pc = curr.address;
                }
                sp--;
                break;
            case pri:
                if (loud) cout<<"[I/O OUT]"<<endl;
                cout<<dstack[sp]<<endl;
                sp--;
                break;
            case hlt: 
                cout<<"![HALT]!"<<endl;
                pc = 0;
                break;
            case lbl:
            case nop:
                pc++;
                break;
            default:
                cout<<"[UNKNOWN INSTRUCTION] [Instruction: "<<curr.inst<<" Address:     "<<curr.address<<"]"<<endl;
                break;
        }
}

 void OwlMachine::doOperation() {
    switch (curr.address) {
        case 0:
            doReturn();
            break;
        case 1:
            dstack[sp] = -dstack[sp];
            break;
        case 2:
            sp--;
            dstack[sp] = dstack[sp] + dstack[sp+1];
            break;
        case 3:
            sp--;
            dstack[sp] = dstack[sp] - dstack[sp+1];
            break;
        case 4:
            sp--;
            dstack[sp] = dstack[sp] * dstack[sp+1];
            break;
        case 5:
            sp--;
            dstack[sp] = dstack[sp] / dstack[sp+1];
            break;
        case 6:
            sp--;
            dstack[sp] = (dstack[sp+1] % 2 == 0);
            break;
        case 8:
            sp--;
            dstack[sp] = (dstack[sp] == dstack[sp+1]);
            break;
        case 9:
            sp--;
            dstack[sp] = (dstack[sp+1] != dstack[sp]);
            break;
        case 10:
            sp--;
            dstack[sp] = (dstack[sp] < dstack[sp+1]);
            break;
        case 11:
            sp--;
            dstack[sp] = (dstack[sp] >= dstack[sp+1]);
            break;
        case 12:
            sp--;
            dstack[sp] = (dstack[sp] > dstack[sp+1]);
            break;
        case 13:
            sp--;
            dstack[sp] = (dstack[sp] <= dstack[sp+1]);
            break;
        default:
            cout<<"[UNKNOWN OPERATION]"<<"  Instruction: "<<curr.inst<<"  Address: "<<curr.address<<"  PC: "<<pc<<endl;
            break;
    }
}

#endif