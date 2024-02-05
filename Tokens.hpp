#ifndef tokens_hpp
#define tokens_hpp
#include <iostream>
using namespace std;

enum TokenType {
    NONE,ERROR,ENDFILE,
    IF,DO,ELSE,ENDIF,WHILE,FUNC,READ,PRINT,BEGIN,END, LET,
    ID,NUM,
    ASSIGN,NOTEQUAL,EQUAL,LT,GT,LTE,GTE,NOT,PLUS,MINUS,MULT,DIVD,LPAREN,RPAREN,QUOTE, SEMI
};

string tokenString[] = {
    "NONE","ERROR","ENDFILE",
    "IF","DO","ELSE","ENDIF","WHILE", "FUNC", "READ", "PRINT","BEGIN","END","LET",
    "ID", "NUM",
    "ASSIGN", "NOTEQUAL", "EQUAL", "LT", "GT", "LTE", "GTE","NOT", "PLUS", "MINUS", "MULT", "DIVD", "LPAREN", "RPAREN", "QUOTE", "SEMI"
};

struct Token {
    TokenType tokenval;
    string stringval;
    int numval;
    int lineno;
    Token(TokenType tt = NONE, string sv = "", int nv = -1) {
        tokenval = tt; stringval = sv; numval = nv;
    }
};

#endif