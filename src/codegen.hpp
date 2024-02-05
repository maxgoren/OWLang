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

void init() {
    codeIndex = 0;
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
    if (loud) cout<<s<<endl;
    if (s[0] == '<' && s[1] == '-') return;
    code[codeIndex++] = s;
}

int emitSkip(int places) {
    int i = code.size();
    for (int n = 0; n < places; n++)
        code[codeIndex++] = "<- SKIP";
    return i;
}

void backUp(int addr) {
    codeIndex = addr;
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
    string codeIns, tmpl;
    switch (x->_node.stmt) {
        case ASSIGNSTM: 
            generate(x->child[0]);
            codeIns = "sto 0, " + to_string(st.find(x->attribute.name));
            if (loud) emit("<- save top of stack to mem address " + to_string(st.find(x->attribute.name)));
            break;
        case WHILESTM:
            tmpl = emit_Label();
            cout<<"Added Label: "<<tmpl<<endl;
            emit("<- loop body");
            generate(x->child[1]);
            emit("<- test condition");
            generate(x->child[0]);
            emit("<- jump back to test.");
            codeIns = "jpc 0, " + to_string(getLabelAddr(tmpl));
            break;
        case IFSTM:
            generate(x->child[0]);
            generate(x->child[1]);
            generate(x->child[2]);
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
            generate(x->child[1]);
            generate(x->child[0]);
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
}

#endif