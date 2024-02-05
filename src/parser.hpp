#ifndef PARSER_HPP
#define PARSER_HPP
#include <iostream>
#include "tokens.hpp"
#include "lexer.hpp"
#include "syntaxTree.hpp"
using namespace std;

enum errorType {
    MISMATCH,
    UNKNOWNSYMBOL
};

int ind = 0;

void say(string s) {
    for (int i = 0; i < ind; i++) cout<<"  ";
    cout<<"("<<ind<<") "<<s<<endl;
}

void onEnter(string s) {
    ++ind;
    say(s);
}

void onExit(string s) {
    say(s);
    --ind;
}

class Parser {
    private:
        using node = SyntaxNode;
        using link = node*;
        TokenStream tokenStream;
        TokenStreamIter streamIter;
        Token lookahead() {
            return streamIter.get();
        }
        void printError(errorType et) {
            switch (et) {
                case MISMATCH:
                case UNKNOWNSYMBOL:
                    cout<<"Unexpected symbol encountered on line "<<lookahead().lineno<<": "<<lookahead().stringval<<endl;
                default:
                    cout<<"An unkown error was encountered on line: "<<lookahead().lineno<<endl;

            }
        }
        bool match(TokenType tt) {
            if (tt == lookahead().tokenval) {
                say("Match: " + lookahead().stringval);                
                streamIter.advance();
                return true;
            }
            return false;
        }
        bool expect(TokenType tt) {
            if (match(tt))
                return true;
            printError(MISMATCH);
            return false;
        }
        link program();
        link block();
        link statementSequence();
        link statement();
        link assignStatement();
        link printStatement();
        link whileStatement();
        link ifStatement();
        link condition();
        link expression();
        link alg_expression();
        link term();
        link factor();
    public:
        Parser(TokenStream ts) {
            tokenStream = ts;
            streamIter = tokenStream.getStream();
        }
        link parse() {
            return program();
        }
};

SyntaxNode* Parser::program() {
    link t = nullptr;
    onEnter("program");
    t = block();
    onExit("program");
    return t;
}

SyntaxNode* Parser::block() {
    link t = nullptr;
    onEnter("block");
    if (match(BEGIN)) {
        t = statementSequence();
    }
    onExit("block");
    return t;
}

SyntaxNode* Parser::statementSequence() {
    link t = statement();
    link p = t;
    while (lookahead().tokenval != END) {
        link q = nullptr;
        match(SEMI);
        q = statement();
        if (q != nullptr) {
            if (t == nullptr) {
                t = p = q;
            } else {
                p->next = q;
                p = q;
            }
        }
    }
    return t;
}

SyntaxNode* Parser::statement() {
    link t = nullptr;
    onEnter("statement");
    if (lookahead().tokenval == LET) {
        match(LET);
        t = assignStatement();
    }
    if (lookahead().tokenval == PRINT) {
        t = printStatement();
    }
    onExit("statement");
    return t;
}

SyntaxNode* Parser::assignStatement() {
    onEnter("assignStatement");
    link t = makeStatementNode(ASSIGNSTM);
    if (lookahead().tokenval == ID) {
        t->attribute.name = lookahead().stringval;
        match(ID);
        expect(ASSIGN);
        t->child[0] = expression();
    } else {
        printError(MISMATCH);
    }
    onExit("assignStatement");
    return t;
}

SyntaxNode* Parser::printStatement() {
    link t = makeStatementNode(PRINTSTM);
    onEnter("printStatement");
    match(PRINT);
    t->child[0] = expression();
    onExit("printStatement");
    return t;
}

SyntaxNode* Parser::condition() {
    link t;
    return t;
}

SyntaxNode* Parser::expression() {
    link t = alg_expression();
    onEnter("expression");
    if (lookahead().tokenval == LT || lookahead().tokenval == EQUAL) {
        link p = makeExpressionNode(OP_EXPR);
        p->child[0] = t;
        p->attribute.op = lookahead().tokenval;
        t = p;
        match(lookahead().tokenval);
        t->child[1] = alg_expression();
    }
    onExit("expression");
    return t;
}

SyntaxNode* Parser::alg_expression() {
    link t = term();
    onEnter("alg_expression");
    while (lookahead().tokenval == PLUS || lookahead().tokenval == MINUS) {
        link p = makeExpressionNode(OP_EXPR);
        p->child[0] = t;
        p->attribute.op = lookahead().tokenval;
        t = p;
        match(lookahead().tokenval);
        t->child[1] = term();
    }
    onExit("alg_expression");
    return t;
}

SyntaxNode* Parser::term() {
    link t;
    onEnter("term");
    t = factor();
    while (lookahead().tokenval == MULT || lookahead().tokenval == DIVD) {
        link p = makeExpressionNode(OP_EXPR);
        p->child[0] = t;
        p->attribute.op = lookahead().tokenval;
        t = p;
        match(lookahead().tokenval);
        t->child[1] = factor();
    }
    onExit("term");
    return t;
}

SyntaxNode* Parser::factor() {
    link t = nullptr;
    onEnter("factor");
    if (lookahead().tokenval == ID) {
        t = makeExpressionNode(ID_EXPR);
        t->attribute.name = lookahead().stringval;
        match(ID);
    } else if (lookahead().tokenval == NUM) {
        t = makeExpressionNode(CONST_EXPR);
        t->attribute.val = lookahead().numval;
        match(NUM);
    } else if (lookahead().tokenval == LPAREN) {
        match(LPAREN);
        t = expression();
        expect(RPAREN);
    } else {
        printError(UNKNOWNSYMBOL);
    }
    onExit("factor");
    return t;
}

#endif
