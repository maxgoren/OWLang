/*

MIT License

Copyright (c) 2024 Max Goren, http://www.maxgcoding.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef SYNTAXTREE_HPP
#define SYNTAXTREE_HPP
#include "../globals/tokens.hpp"
enum NodeType {
    STMTNODE, EXPRNODE
};

enum StatementType {
    IFSTM, ASSIGNSTM, WHILESTM, PRINTSTM,READSTM,FUNCDECL, PROCDCALL
};
vector<string> StmtTypeStr = { "IFSTM", "ASSIGNSTM", "WHILESTM", "PRINTSTM", "READSTM", "FUNCDECL", "PROCDCALL"};

enum ExpressionType {
    CONST_EXPR, OP_EXPR, ID_EXPR
};
vector<string> ExprTypeStr = { "CONST_EXPR", "OP_EXPR", "ID_EXPR"};

const int MAXCHILD = 3;

struct SyntaxNode {
    int lineNumber;
    NodeType nodeKind;
    SyntaxNode *child[MAXCHILD];
    SyntaxNode *next;
    struct { StatementType stmt; ExpressionType expr; } node;
    struct { TokenType op; int val; string name; } attribute;
    SyntaxNode(Token token = NONE) {
        attribute.op = token.tokenval;
        attribute.name = token.stringval;
        attribute.val = token.numval;
    }
};

SyntaxNode* makeStatementNode(StatementType st) {
    SyntaxNode* t = new SyntaxNode();
    t->nodeKind = STMTNODE;
    t->node.stmt = st;
    return t;
}

SyntaxNode* makeExpressionNode(ExpressionType et) {
    SyntaxNode* t = new SyntaxNode();
    t->nodeKind = EXPRNODE;
    t->node.expr = et;
    return t;
}


void nullop(SyntaxNode* x) {
    if (x == nullptr) return;
    return;
}

int mzt = 0;
void printNode(SyntaxNode* x) { 
    if (x != nullptr) {
        if (x->nodeKind == EXPRNODE) cout<<"["<<ExprTypeStr[x->node.expr]<<"]";
        else if (x->nodeKind == STMTNODE) cout<<"["<<StmtTypeStr[x->node.stmt]<<"]";
        else cout<<"[what?]"<<endl;
        cout<<" - { name: "<<((x->attribute.name.size() > 0) ? x->attribute.name:"[0xff]")<<", val: "<<flush;
        cout<<((x->attribute.val >= 0) ? to_string(x->attribute.val):"[0xff]")<<", op: "<<flush;
        cout<<((x->attribute.op > 0) ? tokenString[x->attribute.op]:"[0xff]")<<"}"<<endl;
    }
}

void printTree(SyntaxNode* x) {
    mzt++;
    while (x != nullptr) {
        for (int i = 0; i < mzt; i++) cout<<"  ";
        printNode(x);
        for (int i = 0; i < MAXCHILD; i++)
            printTree(x->child[i]);
        x = x->next;
    }
    mzt--;
}

void traverse(SyntaxNode* x, void (*preOp)(SyntaxNode*), void (*postOp)(SyntaxNode*)) {
    if (x != nullptr) {
        preOp(x);
        for (int i = 0; i < MAXCHILD; i++)
            traverse(x->child[i], preOp, postOp);
        postOp(x);
        traverse(x->next, preOp, postOp);
    }
}

#endif
