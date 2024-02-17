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
    NORMAL, FUNCPARAM, DONE
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

map<string, string> procedureLabels;
SymbolTable st;
int memAddr = 0;
bool loud = false;
vector<Instruction> code(1000);
int codeIndex;
int highCI;
int labelNum;

void init() {
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
    cout<<"Finding "<<l;
    for (; i <= highCI; i++) {
        cout<<".";
        if (code[i].label == l) {
            cout<<" Found at address: "<<i<<endl;
            return i;
        }
    }
    cout<<"Uh Oh.."<<endl;
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


void assignAddrToVarNames(SyntaxNode* x) {
    switch (x->nodeKind) {
        case STMTNODE:
        if (x->node.stmt == ASSIGNSTM || x->node.stmt == FUNCDECL) {
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
    string codeIns, while_body_label, while_test_label, loop_exit_label;
    string if_body_label, else_body_label;
    string func_body_label;
    SyntaxNode* itr;
    int s1, s2, cl, num_vars;
    switch (x->node.stmt) {
        case ASSIGNSTM:
            generate(x->child[0]);
            emit(STO,st.find(x->attribute.name),level());
            break;
        case WHILESTM:
            while_test_label = emit_Label();
            generate(x->child[0]);
            s1 = emitSkip(1);
            while_body_label = emit_Label();
            generate(x->child[1]);
            emit(JMP, getLabelAddr(while_test_label), 0);
            loop_exit_label = emit_Label();
            backUpEmit(s1);
            emit(JNE, getLabelAddr(loop_exit_label), 0);
            restoreEmit();
            break;
        case IFSTM:
            generate(x->child[0]);
            s1 = emitSkip(1);
            generate(x->child[1]);
            s2 = emitSkip(1);
            cl = emitSkip(0);
            backUpEmit(s1);
            emit(JNE, cl, 0);
            restoreEmit();
            generate(x->child[2]);
            cl = emitSkip(0);
            backUpEmit(s2);
            emit(JMP, cl, 0);
            restoreEmit();
            break;
        case PRINTSTM:
            generate(x->child[0]);
            emit(PRI, 0, 0);
            break;
        case FUNCDECL:
            s1 = emitSkip(2);
            emit(ENT, st.find(x->attribute.name), countSiblings(x->child[1]));
            setState(NORMAL);
            generate(x->child[1]);
            emit(RET,0,0);
            cl = emitSkip(0);
            backUpEmit(s1);
            emit(JMP, cl, 0);
            func_body_label = emit_Label();
            procedureLabels[x->attribute.name] = func_body_label;
            restoreEmit();
            break;
        case RETURNSTM:
            generate(x->child[0]);
            break;
        default:
            cout<<"[ WHAT? ]"<<endl;
            break;
    }
}

void generateExpression(SyntaxNode* x) {
    string codeIns;
    switch(x->node.expr) {
        case CONST_EXPR: 
            emit(LDC, 0, x->attribute.val);
            break;
        case ID_EXPR: 
            emit(LOD, st.find(x->attribute.name), level());
            break;
        case OP_EXPR: 
            generate(x->child[0]);
            generate(x->child[1]);
            emitInstruction(x->attribute.op);
            break;
        case PROCDCALL:
            setState(FUNCPARAM);
            emit(MST, 0, 0);
            generate(x->child[1]);
            emit(CALL, getLabelAddr(procedureLabels[x->attribute.name]), 0);
            break;
        default:
            cout<<"[!! WHAT? !!]"<<endl;
            break;
    }
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


vector<Instruction> generatePCodeFromAST(SyntaxNode* x, bool trace) {
    loud = trace;
    setState(NORMAL);
    init();
    traverse(x,  &assignAddrToVarNames, &nullop);
    generate(x);
    emit(HLT, 0, 0);
    setState(DONE);
    st.dumpTable();
    for (int i = 0; i < codeIndex; i++) {
        cout<<setw(2)<<i<<": ";
        code[i].output(cout);
    }
    return code;
}

#endif