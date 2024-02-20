#ifndef codegen_hpp
#define codegen_hpp
#include <iostream>
#include <vector>
#include <iomanip>
#include <map>
#include "../globals/tokens.hpp"
#include "../ast/syntaxTree.hpp"
#include "../symbolTable/symbolTable.hpp"
#include "vm/owlvm_ds.hpp"
using namespace std;

enum CodeGenState {
    NORMAL, FUNCPARAM, FUNCBODY, DONE
};

CodeGenState progress = DONE;

int level() {
    if (progress == FUNCPARAM)
        return 1;
    return 0;
}

void setState(CodeGenState st) {
    progress = st;
}

CodeGenState getState() {
    return progress;
}

//API for code generation
vector<Instruction>  generatePCodeFromAST(SyntaxNode* x, bool trace);
vector<Instruction>  getCode();


//internal methods for codegen
void            init();
void            generatePCode(SyntaxNode* x, bool isAddr);
void            generateExpression(SyntaxNode* x, bool isAddr);
void            generateStatement(SyntaxNode* x, bool isAddr);

//methods for outputting P-Code
void           emit();
int            emitSkip(int n);
string         emit_Label();
void           backUpEmit(int n);
void           restoreEmit();
void           addInstructionToEmit(TokenType tt, string& opstr);
void           assignAddrToVarNames(SyntaxNode* x);
int            getLabelAddr(string lbl);

map<string, string> procedureLabels;
SymbolTable st;
int memAddr;
int globalMemAddr;
bool loud = false;
vector<Instruction> code(1000);
int codeIndex;
int highCI;
int labelNum;

void init() {
    memAddr = 0;
    globalMemAddr = 2499;
    codeIndex = 0;
    highCI = 0;
    labelNum = 0;
    st.clear();
}

vector<Instruction> getCode() {
    return code;
}

string emit_Label() {
    string label = "LBL" + to_string(labelNum++) + ":";
    code[codeIndex] = Instruction(LBL, 0, 0);
    code[codeIndex].label = label;
    cout<<"Label placed at: "<<codeIndex<<endl;
    codeIndex++;
    return label;
}

int getLabelAddr(string l) {
    int i = 0;
    cout<<"Locating "<<l;
    for (; i <= highCI; i++) {
        cout<<".";
        if (code[i].label == l) {
            cout<<" address: "<<i<<endl;
            return i;
        }
    }
    return -1;
}

void emit(vmop op, int address, int value) {
    code[codeIndex++] = Instruction(op, address, value);
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

void emitInstruction(TokenType op) {
    switch (op) {
        case PLUS:
            emit(ADI, 0, 0);
            break;
        case MINUS:
            emit(SBI, 0, 0);
            break;
        case MULT:
            emit(MPI, 0, 0);
            break;
        case DIVD:
            emit(DVI, 0, 0);
            break;
        case NOT:
            emit(NEG, 0, 0);
            break;
        case EQUAL:
            emit(EQU, 0, 0);
            break;
        case NOTEQUAL:
            emit(NEQ, 0, 0);
            break;
        case LESS:
            emit(LT, 0, 0);
            break;
        case LESSEQ:
            emit(LTE, 0, 0);
            break;
        case GREATER:
            emit(GT, 0, 0);
            break;
        case GREATEREQ:
            emit(GTE, 0, 0);
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

void generateStatement(SyntaxNode* x, bool isAddr) {
    if (isAddr) cout<<"isAddr enabled"<<endl;
    string codeIns, while_body_label, while_test_label, loop_exit_label;
    string if_body_label, else_body_label;
    string func_body_label;
    SyntaxNode* itr;
    int s1, s2, cl, num_vars;
    switch (x->node.stmt) {
        case ARRAYSTM:
        case EXPRSTM:
            generatePCode(x->child[0], false);
            generatePCode(x->child[1], false);
            break;
        case ASSIGNSTM:
            if (isAddr) emit(LDA, st.find(x->attribute.name), level());
            generatePCode(x->child[0], true);
            generatePCode(x->child[1], false);
            emit(STO, 0, level());
            break;
        case WHILESTM:
            while_test_label = emit_Label();
            generatePCode(x->child[0], false);
            s1 = emitSkip(1);
            while_body_label = emit_Label();
            generatePCode(x->child[1], false);
            emit(JMP, getLabelAddr(while_test_label), 0);
            loop_exit_label = emit_Label();
            backUpEmit(s1);
            emit(JNE, getLabelAddr(loop_exit_label), 0);
            restoreEmit();
            break;
        case IFSTM:
            generatePCode(x->child[0], false);
            s1 = emitSkip(1);
            generatePCode(x->child[1], false);
            s2 = emitSkip(1);
            cl = emitSkip(0);
            backUpEmit(s1);
            emit(JNE, cl, 0);
            restoreEmit();
            generatePCode(x->child[2], false);
            cl = emitSkip(0);
            backUpEmit(s2);
            emit(JMP, cl, 0);
            restoreEmit();
            break;
        case PRINTSTM:
            generatePCode(x->child[0], false);
            emit(PRI, 0, 0);
            break;
        case FUNCDECL:
            s1 = emitSkip(2);
            emit(ENT, st.find(x->attribute.name), countSiblings(x->child[1]));
            setState(FUNCBODY);
            generatePCode(x->child[1],  false);
            emit(RET,0,0);
            cl = emitSkip(0);
            backUpEmit(s1);
            emit(JMP, cl, 0);
            func_body_label = emit_Label();
            procedureLabels[x->attribute.name] = func_body_label;
            restoreEmit();
            setState(NORMAL);
            break;
        case RETURNSTM:
            generatePCode(x->child[0], false);
            break;
        case VARDECLSTM:
            cout<<"Var: "<<x->attribute.name<<" declared."<<endl;
            break;
        default:
            cout<<"[ WHAT? ]"<< x->attribute.name<<" " <<endl;
            cout<<"["<<StmtTypeStr[x->node.stmt]<<"]"<<endl;
            break;
    }
}

void generateExpression(SyntaxNode* x, bool isAddr) {
    string codeIns;
    CodeGenState preState = getState();
    switch(x->node.expr) {
        case CONST_EXPR: 
            emit(LDC, 0, x->attribute.val);
            break;
        case SUBSCIPT_EXPR:
            if (isAddr) cout<<"isAddr enabled"<<endl;
                printNode(x);
                emit(LDA, st.find(x->child[0]->attribute.name), level());
                generatePCode(x->child[0], false);
                emit(IXA, st.find(x->child[0]->attribute.name), level());
                if (!isAddr) emit(IDL, 0, 0);
                break;
        case ID_EXPR:
            cout<<"ID: "<<x->attribute.name<<endl;
            if (isAddr) {
                emit(LDA, st.find(x->attribute.name), level());
            } else {
                emit(LOD, st.find(x->attribute.name), level());
            }
            break;
        case OP_EXPR: 
            generatePCode(x->child[0], false);
            generatePCode(x->child[1], false);
            emitInstruction(x->attribute.op);
            break;
        case PROCDCALL:
            setState(FUNCPARAM);
            emit(MST, 0, 0);
            generatePCode(x->child[1], false);
            if (preState == FUNCBODY) {
                int checkFrom = emitSkip(0);
                while (checkFrom >= 0) {
                    if (code[checkFrom].op == ENT) {
                        break;
                    }
                    checkFrom--;
                }
                emit(CALL, checkFrom, 0);
            } else {
                emit(CALL, getLabelAddr(procedureLabels[x->attribute.name])-1, 0);
            }
            break;
        default:
            cout<<"[!! WHAT? !!]"<<endl;
            break;
    }
}

void generatePCode(SyntaxNode* x, bool isAddr) {
    if (x == nullptr) return;
    string codeIns;
    if (x->nodeKind == EXPRNODE) {
        generateExpression(x, isAddr);
    } else if (x->nodeKind == STMTNODE) {
        generateStatement(x, isAddr);
    }
    generatePCode(x->next, isAddr);
}

void assignAddrToVarNames(SyntaxNode* x) {
    switch (x->nodeKind) {
        case STMTNODE:
            if (x->node.stmt == FUNCDECL) {
                if (st.find(x->attribute.name) == -1) {
                    st.insert(x->attribute.name, memAddr++);
                    if (loud) cout<<"Procedure Name: "<<x->attribute.name<<" assigned forwarding address "<<memAddr - 1<<endl;
                }
            } else if (x->node.stmt == ASSIGNSTM) {
                if (st.find(x->attribute.name) == -1) {
                    switch(x->attribute.store) {
                        case LOCAL:
                            st.insert(x->attribute.name, memAddr++);
                            break;
                        case GLOBAL:
                            st.insert(x->attribute.name, globalMemAddr--);
                            break;
                        default:
                            break;
                    }
                    if (loud) cout<<"Assigned variable: "<<x->attribute.name<<" to memory address "<<(x->attribute.store == LOCAL ? memAddr-1:globalMemAddr+1)<<endl;
                }
            } else if (x->node.stmt == ARRAYSTM) {
                if (st.find(x->attribute.name) == -1) {
                    switch(x->attribute.store) {
                        case LOCAL:
                            st.insert(x->attribute.name, memAddr++);
                            memAddr += x->attribute.val;
                            break;
                        case GLOBAL:
                            st.insert(x->attribute.name, globalMemAddr--);
                            globalMemAddr -= x->attribute.val;
                            break;
                        default:
                            break;
                    }
                    if (loud) cout<<"Assigned Array: "<<x->attribute.name<<" to memory address "<<(x->attribute.store == LOCAL ? memAddr-1:(globalMemAddr+x->attribute.val+1))<<endl;
                }
            }
        return;
    case EXPRNODE:
            if (x->node.expr == ID_EXPR) {
                if (st.find(x->attribute.name) == -1) {
                    switch(x->attribute.store) {
                        case LOCAL:
                            st.insert(x->attribute.name, memAddr++);
                            break;
                        case GLOBAL:
                            st.insert(x->attribute.name, globalMemAddr--);
                            break;
                        default:
                            break;
                    }
                    if (loud) cout<<"Assigned variable: "<<x->attribute.name<<" to memory address "<<(x->attribute.store == LOCAL ? memAddr-1:globalMemAddr+1)<<endl;
                }
            }
            break;
        default:
            break;
    }
}


vector<Instruction> generatePCodeFromAST(SyntaxNode* x, bool trace) {
    loud = trace;
    setState(NORMAL);
    init();
    traverse(x,  &assignAddrToVarNames, &nullop);
    generatePCode(x, false);
    emit(HLT, 0, 0);
    setState(DONE);
    st.dumpTable();
    for (int i = 0; i < codeIndex; i++) {
        if (code[i].op == CALL && code[i].addr == -1) {
            int j = i;
            while (j > 0 && code[j].op != ENT) j--;
            code[i].addr = j;
        }
        cout<<setw(2)<<i<<": ";
        code[i].output(cout);
    }
    return code;
}

#endif