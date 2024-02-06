#ifndef codegen_hpp
#define codegen_hpp
#include <iostream>
#include <vector>
#include "Tokens.hpp"
#include "syntaxtree.hpp"
#include "symbolTable.hpp"
using namespace std;


void addInstructionToEmit(TokenType tt, string& opstr);
SymbolTable st;
int memAddr = 0;
bool loud = false;
vector<string> code(1000);
int codeIndex;
int highCI;

void init() {
    codeIndex = 0;
    highCI = 0;
}

vector<string> getCode() {
    return code;
}

int labelNum = 0;
string emit_Label() {
    string label = "LBL" + to_string(labelNum) + ":";
    code[codeIndex++] = label;
    return label;
}

int getLabelAddr(string l) {
    int i = 0;
    cout<<"Finding "<<l;
    for (; i < codeIndex; i++) {
        cout<<".";
        if (code[i] == l) {
            cout<<" Found at address: "<<i<<endl;
            return i;
        }
    }
    cout<<"Uh Oh.."<<endl;
    return -1;
}

void emit(string s) {
    if (loud) cout<<"codeIndex: "<<codeIndex<<", "<<s<<endl;
    if (s[0] == '<' && s[1] == '-') return;
    code[codeIndex++] = s;
    if (codeIndex > highCI) highCI = codeIndex;
}

int emitSkip(int places) {
    int i = codeIndex;
    codeIndex += places;
    if (highCI < codeIndex) highCI = codeIndex;
    cout<<"Emit Skip at: "<<i<<", new CI: "<<codeIndex<<endl;
    return i;
}

void backUpEmit(int addr) {
    if (addr > highCI) cout<<"Something screwy going on..."<<endl;
    codeIndex = addr;
}

void restoreEmit() {
    codeIndex = highCI;
}

void addInstructionToEmit(TokenType tt, string& opstr) {
    switch (tt) {
        case PLUS: 
            opstr.push_back('2');
            if (loud)  
                emit("<- add"); 
            break;
        case MINUS: 
            opstr.push_back('3');
            if (loud)  
                emit("<- subtract"); 
            break;
        case MULT: 
            opstr.push_back('4'); 
            if (loud) 
                emit("<- multiply");
            break;
        case DIVD: 
            opstr.push_back('5'); 
            if (loud) 
                emit("<- divide"); 
            break;
        case NOT: 
            opstr.push_back('6'); 
            if (loud) 
                emit("<- negate"); 
            break;
        case EQUAL: 
            opstr.push_back('8'); 
            if (loud) 
                emit("<- Equal");
            break;
        case NOTEQUAL: 
            opstr.push_back('9');
            if (loud)  
                emit("<- not equal");
            break;
        case LT: 
            opstr += "10";
            if (loud) 
                emit("<- less than"); 
            break;
        default:
         opstr.push_back('z');
         break;
    }
}

void buildST(SyntaxNode* x) {
    if (x->nodeType == STMTNODE) {
        if (x->_node.stmt == ASSIGNSTM) {
            if (st.find(x->attribute.name) == -1) {
                st.insert(x->attribute.name, memAddr++);
                if (loud) cout<<"Assigned variable: "<<x->attribute.name<<" to memory address "<<memAddr - 1<<endl;
            }
        }
        return;
    }
    if (x->nodeType == EXPRNODE) {
        if (x->_node.expr == ID_EXPR) {
            if (st.find(x->attribute.name) == -1)
                st.insert(x->attribute.name, memAddr++);
        }
    }
}

void generate(SyntaxNode* x);

void generateStatement(SyntaxNode* x) {
    string codeIns, while_body_label;
    string if_body_label, else_body_label;
    int s1, s2, cl;
    switch (x->_node.stmt) {
        case ASSIGNSTM: 
            generate(x->child[0]);
            codeIns = "sto 0, " + to_string(st.find(x->attribute.name));
            if (loud) emit("<- save top of stack to mem address " + to_string(st.find(x->attribute.name)));
            break;
        case WHILESTM:
            while_body_label = emit_Label();
            cout<<"Added Label: "<<while_body_label<<endl;
            s1 = emitSkip(0);
            emit("<- test condition");
            generate(x->child[1]);
            emit("<- loop body");
            generate(x->child[0]);
            emit("<- jump back to test.");
            codeIns = "jeq 0, " + to_string(getLabelAddr(while_body_label));
            break;
        case IFSTM:
            //test
            generate(x->child[0]);
            s1 = emitSkip(1);
            generate(x->child[1]);
            s2 = emitSkip(1);
            cl = emitSkip(0);
            backUpEmit(s1);
            codeIns = "jpc 0, " + to_string(cl);
            emit(codeIns);
            restoreEmit();
            generate(x->child[2]);
            cl = emitSkip(0);
            backUpEmit(s2);
            codeIns = "jmp 0, " + to_string(cl);
            emit(codeIns);
            restoreEmit();
            return;
            break;
        case PRINTSTM:
            generate(x->child[0]);
            codeIns = "pri"; 
            break;
        default:
            cout<<"[!! WHAT? !!]"<<endl;
            break;
    }
    emit(codeIns);
}

void generateExpression(SyntaxNode* x) {
    string codeIns;
    switch(x->_node.expr) {
        case CONST_EXPR: 
            codeIns = "lit 0, " + to_string(x->attribute.val);
            if (loud) 
                emit("<- push " + to_string(x->attribute.val));
            break;
        case ID_EXPR: 
            codeIns = "lod 0, " + to_string(st.find(x->attribute.name));
            if (loud)
                emit("<- push contents of mem address " + to_string(st.find(x->attribute.name)) + " to top of stack");
            break;
        case OP_EXPR: 
            generate(x->child[0]);
            generate(x->child[1]);
            codeIns = "opr 0, ";
            addInstructionToEmit(x->attribute.op, codeIns);
            break;
        default:
            cout<<"[!! WHAT? !!]"<<endl;
            break;
    }
    emit(codeIns);
}

void generate(SyntaxNode* x) {
    if (x == nullptr) return;
    string codeIns;
    if (x->nodeType == EXPRNODE) {
        generateExpression(x);
    } else if (x->nodeType == STMTNODE) {
        generateStatement(x);
    }
    generate(x->next);
}


void generateCode(SyntaxNode* x, bool trace) {
    loud = trace;
    init();
    traverse(x,  &buildST, &nullop);
    generate(x);
    code[codeIndex++] = "hlt";
    for (int i = 0; i < codeIndex; i++)
        cout<<code[i]<<endl;
}

#endif