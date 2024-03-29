#ifndef PARSER_HPP
#define PARSER_HPP
#include <iostream>
#include "../globals/tokens.hpp"
#include "../ast/syntaxTree.hpp"
#include "../symboltable/symbolTable.hpp"
#include "lexer.hpp"
using namespace std;

enum errorType {
    MISMATCH,
    UNKNOWNSYMBOL
};

bool traceParse;
int ind = 0;

void say(string s) {
    if (!traceParse) return;
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


bool flip_type = false;
enum ParserState {
    GLOBAL_BODY, PROCEDURE_BODY
};

class OwlParser {
    private:
        ParserState parseState;
        void setParserState(ParserState ps) {
            parseState = ps;
        }
        using node = SyntaxNode;
        using link = node*;
        int matchFail;
        TokenStream tokenStream;
        TokenStreamIter streamIter;
        SymbolTable procedureNames;
        Token lookahead() {
            return streamIter.get();
        }
        void printError(errorType et) {
            for (int i = 0; i < ind; i++) cout<<"  ";
            switch (et) {
                case MISMATCH:
                case UNKNOWNSYMBOL:
                    cout<<" - Unexpected symbol encountered on line "<<lookahead().lineno<<": "<<lookahead().stringval<<endl;
                    return;
                    break;
                default:
                    cout<<" - An unkown error was encountered on line: "<<lookahead().lineno<<endl;

            }
        }
        bool match(TokenType tt) {
            if (tt == lookahead().tokenval) {
                say("Match: " + lookahead().stringval);                
                streamIter.advance();
                return true;
            }
            matchFail++;
            cout<<"Match Failure: "<<lookahead().stringval<<endl;
            if (matchFail > 5) {
                cout<<"Too many mismatches, bailing out."<<endl;
                exit(0);
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
        link declareArrayStatement();
        link declareFuncStatement();
        link declareVarStatement();
        link returnStatement();
        link printStatement();
        link whileStatement();
        link ifStatement();
        link exprStatement();
        link paramList();
        link funcCall();
        link argList();
        link expression();
        link alg_expression();
        link var();
        link varDecl();
        link term();
        link factor();
    public:
        OwlParser(TokenStream ts, bool trace);
        OwlParser();
        link parse();
        link parse(TokenStream ts, bool trace);
};

OwlParser::OwlParser(TokenStream ts, bool trace) {
    matchFail = 0;
    traceParse = trace;
    tokenStream = ts;
    streamIter = tokenStream.getStream();
}

OwlParser::OwlParser() {
    traceParse = false;
    matchFail = 0;
}

SyntaxNode* OwlParser::parse() {
    return program();
}

SyntaxNode* OwlParser::parse(TokenStream ts, bool trace) {
    traceParse = trace;
    tokenStream = ts;
    streamIter = tokenStream.getStream();
    return program();
}

SyntaxNode* OwlParser::program() {
    link t = nullptr;
    onEnter("program");
    setParserState(GLOBAL_BODY);
    match(PROG);
    match(SQUOTE);
    match(ID);
    match(SQUOTE);
    match(SEMI);
    t = block();
    onExit("program");
    return t;
}

SyntaxNode* OwlParser::block() {
    link t = nullptr;
    onEnter("block");
    if (match(BEGIN)) {
        t = statementSequence();
    }
    match(END);
    onExit("block");
    return t;
}

SyntaxNode* OwlParser::statementSequence() {
    link t = statement();
    link p = t;
    while ((lookahead().tokenval != END) && (lookahead().tokenval != ELSE)) {
        link q = nullptr;
        if (match(SEMI)) {
            if (lookahead().tokenval == END) break;
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
    }
    return t;
}

SyntaxNode* OwlParser::statement() {
    link t = nullptr;
    onEnter("statement");
    switch (lookahead().tokenval) {
        case LET:
            match(LET);
            t = declareVarStatement();
            break;
        case WHILE:
            match(WHILE);
            t = whileStatement();
            break;
        case IF:
            match(IF);
            t = ifStatement();
            break;
        case RETURN:
            match(RETURN);
            t = returnStatement();
            break;
        case PRINT:
            match(PRINT);
            t = printStatement();
            break;
        case FUNC:
            match(FUNC);
            t = declareFuncStatement();
            break;
        case ID:
            t = exprStatement();
            break;
        default:
            printError(UNKNOWNSYMBOL);
            break;
    }
    onExit("statement");
    return t;
}

SyntaxNode* OwlParser::declareFuncStatement() {
    onEnter("declareFuncStatement");
    link t = makeStatementNode(FUNCDECL);
    t->attribute.name = lookahead().stringval;
    match(ID);
    cout<<"Added "<<t->attribute.name<<" to list of procedure names."<<endl;
    procedureNames.insert(t->attribute.name, 0);
    match(LPAREN);
    setParserState(PROCEDURE_BODY);
    t->child[0] = paramList();
    match(RPAREN);
    t->child[1] = block();
    setParserState(GLOBAL_BODY);
    onExit("declareFuncStatement");
    return t;
}

SyntaxNode* OwlParser::declareVarStatement() {
    link t = makeStatementNode(VARDECLSTM);
    onEnter("declareVarStatement");
    t->attribute.name = lookahead().stringval;
    t->attribute.store = parseState == PROCEDURE_BODY ? LOCAL:GLOBAL;
    match(ID);
    if (lookahead().tokenval == LSQBRACKET) {
        match(LSQBRACKET);
        t->attribute.val = lookahead().numval;
        match(NUM);
        match(RSQBRACKET);
    }
    match(COLON);
    if (lookahead().tokenval == INT) {
        match(INT);
    } else if (lookahead().tokenval == ARRAY) {
        match(ARRAY);
    }
    onExit("declareVarStatement");
    return t;
}


SyntaxNode* OwlParser::exprStatement() {
    onEnter("exprStatement");
    link t = makeStatementNode(EXPRSTM);
    if (lookahead().tokenval == ID) {
        t->child[0] = var();
        if (lookahead().tokenval == ASSIGN) {
            link p = makeStatementNode(ASSIGNSTM);
            p->attribute.op = ASSIGN;
            match(ASSIGN);
            p->child[0] = t->child[0];
            p->child[1] = expression();
            t->child[0] = p;
        } else if (lookahead().tokenval == LPAREN) {
            link p = makeExpressionNode(PROCDCALL);
            p->attribute.name = t->child[0]->attribute.name;
            match(LPAREN);
            if (lookahead().tokenval != RPAREN) {
                link td = t->child[0];
                p->child[0] = t->child[0];
                p->child[1] = argList();
                p->child[0] = nullptr;
                t->child[0] = p;
                delete td;
            }
            match(RPAREN);
        }
    } else {
        t->child[0] = alg_expression();
    }
    onExit("exprStatement");
    return t;
}

SyntaxNode* OwlParser::var() {
    link t = makeExpressionNode(ID_EXPR);
    onEnter("var");
    t->attribute.name = lookahead().stringval;
    t->attribute.store = parseState == PROCEDURE_BODY ? LOCAL:GLOBAL;
    if (lookahead().tokenval == ID) {
        match(ID);
        if (lookahead().tokenval == LSQBRACKET) {
            match(LSQBRACKET);
            t->child[0] = expression();
            match(RSQBRACKET);
            t->child[0]->node.expr = ID_EXPR;
            t->node.expr = SUBSCIPT_EXPR;
        }
    }
    onExit("var");
    return t;
}

SyntaxNode* OwlParser::returnStatement() {
    link t = makeStatementNode(RETURNSTM);
    onEnter("return statement");
    t->child[0] = expression();
    onExit("return statement");
    return t;
}

SyntaxNode* OwlParser::printStatement() {
    link t = makeStatementNode(PRINTSTM);
    onEnter("printStatement");
    t->child[0] = expression();
    onExit("printStatement");
    return t;
}

SyntaxNode* OwlParser::whileStatement() {
    link t = makeStatementNode(WHILESTM);
    onEnter("whileStatement");
    t->child[0] = expression();
    match(BEGIN);
    t->child[1] = statementSequence();
    match(END);
    onExit("whileStatement");
    return t;
}

SyntaxNode* OwlParser::ifStatement() {
    link t = makeStatementNode(IFSTM);
    onEnter("ifStatement");
    t->child[0] = expression();
    match(THEN);
    t->child[1] = statementSequence();
    if (lookahead().tokenval == ELSE) {
        match(ELSE);
        t->child[2] = statementSequence();
    }
    match(END);
    onExit("ifStatement");
    return t;
}


SyntaxNode* OwlParser::paramList() {
    link t = nullptr;
    onEnter("paramList");
    if (lookahead().tokenval == RPAREN) {
        return t;
    } else {
        t = factor();
        link m = t;
        while (lookahead().tokenval == COMA) {
            match(COMA);
            link p = factor();
            m->next = p;
            m = p;
        }
    }
    onExit("paramList");
    return t;
}

SyntaxNode* OwlParser::funcCall() {
    onEnter("funcCall");
    link t = makeExpressionNode(PROCDCALL);
    t->attribute.name = lookahead().stringval;
    match(LPAREN);
    if (lookahead().tokenval != RPAREN)
        t->child[1] = argList();
    match(RPAREN);
    onExit("funcCall");
    return t;
}

SyntaxNode* OwlParser::argList() {
    link t = nullptr;
    onEnter("argList");
    if (lookahead().tokenval == RPAREN) {
        match(RPAREN);
    } else {
        t = expression();
        link m = t;
        while (lookahead().tokenval == COMA) {
            match(COMA);
            link p = expression();
            m->next = p;
            m = p;
        }
    }
    onExit("argList");
    return t;
}

SyntaxNode* OwlParser::expression() {
    link t = alg_expression();
    onEnter("expression");
    if (lookahead().tokenval == LESS || lookahead().tokenval == EQUAL) {
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

SyntaxNode* OwlParser::alg_expression() {
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

SyntaxNode* OwlParser::term() {
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


SyntaxNode* OwlParser::factor() {
    link t = nullptr;
    onEnter("factor");
    if (lookahead().tokenval == ID && procedureNames.find(lookahead().stringval) != -1) {
        t = funcCall();
    } else if (lookahead().tokenval == ID) {
        t = var();
    } else if (lookahead().tokenval == NUM) {
        t = makeExpressionNode(CONST_EXPR);
        t->attribute.val = lookahead().numval;
        match(NUM);
    } else if (lookahead().tokenval == LPAREN) {
        match(LPAREN);
        if (lookahead().tokenval != RPAREN)
            t = expression();
        expect(RPAREN);
    } else {
        printError(UNKNOWNSYMBOL);
    }
    onExit("factor");
    return t;
}

#endif
