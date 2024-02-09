#ifndef codegen_hpp
#define codegen_hpp
#include <iostream>
#include <vector>
#include "Tokens.hpp"
#include "syntaxtree.hpp"
#include "symbolTable.hpp"
using namespace std;

//API for code generation
vector<string>  generatePCodeFromAST(SyntaxNode* x, bool trace);
vector<string>  getCode();


//internal methods for codegen
void            init();
void            generate(SyntaxNode* x);
void            generateExpression(SyntaxNode* x);
void            generateStatement(SyntaxNode* x);

//methods for outputting P-Code
void           emit();
int            emitSkip(int n);
string         emit_Label();
void           backUpEmit(int n);
void           restoreEmit();
void           addInstructionToEmit(TokenType tt, string& opstr);
void           assignAddrToVarNames(SyntaxNode* x);
int            getLabelAddr(string lbl);

SymbolTable st;
int memAddr = 0;
bool loud = false;
vector<string> code(1000);
int codeIndex;
int highCI;
int level;
void init() {
    codeIndex = 0;
    highCI = 0;
    level = 0;
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
    if (s[0] == '<' && s[1] == '-') return;
    code[codeIndex++] = s;
    if (highCI < codeIndex) highCI = codeIndex;
}

int emitSkip(int places) {
    int i = codeIndex;
    codeIndex += places;
    if (highCI < codeIndex) highCI = codeIndex;
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
        case GT:
            opstr += "11";
            if (loud)
                emit("<- greater than");
            break;
        default:
         opstr.push_back('z');
         break;
    }
}

void assignAddrToVarNames(SyntaxNode* x) {
    switch (x->nodeKind) {
        case STMTNODE:
        if (x->node.stmt == ASSIGNSTM) {
            if (st.find(x->attribute.name) == -1) {
                st.insert(x->attribute.name, memAddr++);
                if (loud) cout<<"Assigned variable: "<<x->attribute.name<<" to memory address "<<memAddr - 1<<endl;
            }
        }
        return;
    case EXPRNODE:
            if (x->node.expr == ID_EXPR) {
                if (st.find(x->attribute.name) == -1) {
                    st.insert(x->attribute.name, memAddr++);
                }
            }
            break;
        default:
            break;
    }
}

int countSiblings(SyntaxNode* x) {
    int num_sib = 0;
    SyntaxNode* it = x;
    while (it != nullptr) {
        num_sib++;
        it = it->next;
    }
    return num_sib;
}

void generateStatement(SyntaxNode* x) {
    string codeIns, while_body_label;
    string if_body_label, else_body_label;
    string func_body_label;
    SyntaxNode* itr;
    int s1, s2, cl, num_vars;
    switch (x->node.stmt) {
        case ASSIGNSTM: 
            generate(x->child[0]);
            codeIns = "sto 0, " + to_string(st.find(x->attribute.name));
            if (loud) emit("<- save top of stack to mem address " + to_string(st.find(x->attribute.name)));
            emit(codeIns);
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
            emit(codeIns);
            break;
        case IFSTM:
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
            break;
        case PRINTSTM:
            generate(x->child[0]);
            codeIns = "pri"; 
            emit(codeIns);
            break;
        case FUNCDECL:
            s1 = emitSkip(1);
            cl = emitSkip(0);
            level++;
            emit("inc 0, " + to_string(countSiblings(x->child[1])));
            st.insert(x->attribute.name, cl);
            generate(x->child[1]);
            emit("opr 0, 0");
            level--;
            cl = emitSkip(0);
            backUpEmit(s1);
            emit("jmp 0, " + to_string(cl));
            restoreEmit();
            break;
        case PROCDCALL:
            itr = x->child[1];
            while (itr != nullptr) {
                generate(itr);
                itr = itr->next;
            }
            emit("cal 0, " + to_string(st.find(x->attribute.name)));
            break;
        default:
            cout<<"[!! WHAT? !!]"<<endl;
            break;
    }
}

void generateExpression(SyntaxNode* x) {
    string codeIns;
    switch(x->node.expr) {
        case CONST_EXPR: 
            codeIns = "lit 0, " + to_string(x->attribute.val);
            if (loud) 
                emit("<- push " + to_string(x->attribute.val));
            break;
        case ID_EXPR: 
            codeIns = "lod " + to_string(level) + ", " + to_string(st.find(x->attribute.name));
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
    if (x->nodeKind == EXPRNODE) {
        generateExpression(x);
    } else if (x->nodeKind == STMTNODE) {
        generateStatement(x);
    }
    generate(x->next);
}


vector<string> generatePCodeFromAST(SyntaxNode* x, bool trace) {
    loud = trace;
    init();
    traverse(x,  &assignAddrToVarNames, &nullop);
    generate(x);
    code[codeIndex++] = "hlt";
    st.dumpTable();
    for (int i = 0; i < codeIndex; i++)
        cout<<code[i]<<endl;
    return code;
}

#endif