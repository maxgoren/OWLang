#ifndef owlmachine_hpp
#define owlmachine_hpp
#include <iostream>
#include <vector>
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
        enum OM_Inst {
            lit, opr, lod, sto, cal, inc, jmp, jpc, pri, hlt, nop
        };
        OM_Inst getominst(string ins) {
            if (ins == "lit") return lit;
            if (ins == "opr") return opr;
            if (ins == "lod") return lod;
            if (ins == "sto") return sto;
            if (ins == "cal") return cal;
            if (ins == "inc") return inc;
            if (ins == "jmp") return jmp;
            if (ins == "jpc") return jpc;
            if (ins == "pri") return pri;
            if (ins == "hlt") return hlt;
            return nop;
        }
        struct Instruction {
            OM_Inst inst;
            int level;
            int address;
            Instruction(OM_Inst i = nop, int l = 0, int a = 0) {
                inst = i;
                level = l;
                address = a;
            }
        };
        vector<Instruction> code;
        typedef int REGISTER;
        REGISTER top;
        REGISTER base;
        REGISTER pc;
        REGISTER ins;
        static const int stackSize = 2600;
        int dstack[stackSize];
        int memStore[2600];
        int findBase(int level) {
            int b1 = base;
            while (level > 0) {
                b1 = dstack[b1];
                level--;
            }
            return b1;
        }
        void init() {
            cout<<"Starting OWL machine."<<endl;
            top = 0;
            base = 1;
            pc = 0;
            for (int i = 0; i < stackSize; i++) 
                dstack[i] = 0;
        }
        void doReturn() {
            top = base - 1;
            pc = dstack[top + 3];
            base = dstack[top + 2];
        }
        void negate() {
            dstack[top] = -dstack[top];
        }
        void add() {
            top--;
            cout<<dstack[top]<<" + "<<dstack[top+1];
            dstack[top] = dstack[top] + dstack[top+1];
            cout<<", R: "<<dstack[top]<<endl;
        }
        void sub() {
            top--;
            dstack[top] = dstack[top] - dstack[top+1];
        }
        void mul() {
            top--;
            dstack[top] = dstack[top] * dstack[top+1];
        }
        void div() {
            top--;
            dstack[top] = dstack[top] / dstack[top+1];
        }
        void isOdd() {
            dstack[top] = (dstack[top] % 2 == 0);
        }
        void isEqual() {
            top--;
            dstack[top] = (dstack[top] == dstack[top+1]);
        }
        void notEqual() {
            top--;
            dstack[top] = (dstack[top] != dstack[top+1]);
        }
        void isLess() {
            top--;
            dstack[top] = (dstack[top] < dstack[top+1]);
        }
        void isGreater() {
            top--;
            dstack[top] = (dstack[top] > dstack[top+1]);
        }
        void isLessOrEqual() {
            top--;
            dstack[top] = (dstack[top] <= dstack[top+1]);
        }
        void isGreaterOrEqual() {
            top--;
            dstack[top] = (dstack[top] > dstack[top+1]);
        }
        void doOperation(Instruction curr) {
            switch (curr.address) {
                case 0: //return
                    doReturn();
                    break;
                case 1:
                    negate();
                    break;
                case 2:
                    add();
                    break;
                case 3:
                    sub();
                    break;
                case 4:
                    mul();
                    break;
                case 5:
                    div();
                    break;
                case 6:
                    isOdd();
                    break;
                case 8:
                    isEqual();
                    break;
                case 9:
                    notEqual();
                    break;
                case 10:
                    isLess();
                    break;
                case 11:
                    isGreaterOrEqual();
                    break;
                case 12:
                    isGreater();
                    break;
                case 13:
                    isLessOrEqual();
                    break;
                default:
                    cout<<"!![UNKNOWN OPERATION]!!"<<endl;
                    cout<<"Instruction: "<<curr.inst<<endl;
                    cout<<"Address:     "<<curr.address<<endl;
                    break;
            }
        }
    public:
        OwlMachine() {
            init();
        }
        void start(bool loud) {
            Instruction curr;
            int i = 0;
            cout<<"[OWLVM v0.1 Starting...]"<<endl;
            do {
                curr = code[pc++];
                if (loud)
                    cout<<"["<<i++<<"]"<<curr.inst<<" "<<curr.level<<", "<<curr.address<<endl;
                
                switch (curr.inst) {
                    case lit: 
                        dstack[++top] = curr.address;
                        break;
                    case opr: 
                        doOperation(curr);
                        break;
                    case lod:
                        top++;
                        dstack[top] = dstack[findBase(curr.level) + curr.address];
                        break;
                    case sto:
                        dstack[findBase(curr.level) + curr.address] = dstack[top];
                        break;
                    case cal:
                        dstack[top + 1] = findBase(curr.level);
                        dstack[top + 2] = base;
                        dstack[top + 3] = pc;
                        base = top + 1;
                        pc = curr.address;
                        break;
                    case inc:
                        top = top + curr.address;
                        break;
                    case jmp:
                        pc = curr.address;
                        break;
                    case jpc:
                        if (dstack[top] == 0) {
                            pc = curr.address;
                            top--;
                        }
                    case pri:
                        cout<<dstack[top]<<endl;
                        break;
                    case hlt: 
                        pc = 0;
                        break;
                    default:
                        cout<<"!![UNKNOWN INSTRUCTION]!!"<<endl;
                        cout<<"Instruction: "<<curr.inst<<endl;
                        cout<<"Address:     "<<curr.address<<endl;
                        break;
                }
            } while (pc > 0 && pc < code.size());
            cout<<"[OWLVM Done.]"<<endl;
        }
        void loadProgram(vector<string>& asmCode) {
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
                code.push_back(next);
            }
        }
};

#endif