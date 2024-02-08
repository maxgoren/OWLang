#ifndef owlmachin_ins_hpp
#define owlmachine_ins_hpp
#include <iostream>
#include <vector>
using namespace std;

enum OM_Inst {
    lit, opr, lod, sto, cal, inc, jmp, jpc, jeq, pri, hlt, nop, lbl
};
vector<string> ominststr = { "lit", "opr", "lod", "sto", "cal", "inc", "jmp", "jpc", "jeq", "pri", "hlt", "nop", "lbl" };
OM_Inst getominst(string ins) {
    if (ins == "LBL") return lbl;
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
    if (ins == "jeq") return jeq;

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
    void output(ostream& os) {
        os<<ominststr[inst] << " " << level << ", "<<address<<endl;
    }
};

#endif