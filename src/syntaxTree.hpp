#ifndef SYNTAXTREE_HPP
#define SYNTAXTREE_HPP
#include "Tokens.hpp"
enum NodeType {
    STMTNODE, EXPRNODE
};

enum StatementType {
    IFSTM, ASSIGNSTM, WHILESTM, PRINTSTM,READSTM
};
vector<string> StmtTypeStr = { "IFSTM", "ASSIGNSTM", "WHILESTM", "PRINTSTM", "READSTM"};

enum ExpressionType {
    CONST_EXPR, OP_EXPR, ID_EXPR
};
vector<string> ExprTypeStr = { "CONST_EXPR", "OP_EXPR", "ID_EXPR"};

const int MAXCHILD = 4;

struct SyntaxNode {
    int lineNumber;
    NodeType nodeType;
    SyntaxNode *child[MAXCHILD];
    SyntaxNode *next;
    struct { StatementType stmt; ExpressionType expr; } _node;
    struct { TokenType op; int val; string name; } attribute;
    SyntaxNode(Token token = NONE) {
        attribute.op = token.tokenval;
        attribute.name = token.stringval;
        attribute.val = token.numval;
    }
};

SyntaxNode* makeStatementNode(StatementType st) {
    SyntaxNode* t = new SyntaxNode();
    t->nodeType = STMTNODE;
    t->_node.stmt = st;
    return t;
}

SyntaxNode* makeExpressionNode(ExpressionType et) {
    SyntaxNode* t = new SyntaxNode();
    t->nodeType = EXPRNODE;
    t->_node.expr = et;
    return t;
}


void nullop(SyntaxNode* x) {
    if (x == nullptr) return;
    return;
}


void printNode(SyntaxNode* x) { 
    if (x != nullptr) {
        if (x->nodeType == EXPRNODE) cout<<"["<<ExprTypeStr[x->_node.expr]<<"] ";
        else if (x->nodeType == STMTNODE) cout<<"["<<StmtTypeStr[x->_node.stmt]<<"] ";
        cout<<((x->attribute.name.size() > 0) ? x->attribute.name:"")<<", "<<((x->attribute.val >= 0) ? to_string(x->attribute.val):"")<<", "<<((x->attribute.op > 0) ? tokenString[x->attribute.op]:"")<<endl;
    }
}

int mzt = 0;
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
