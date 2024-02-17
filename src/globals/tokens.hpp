#ifndef tokens_hpp
#define tokens_hpp
#include <iostream>
using namespace std;

enum TokenType {
    NONE,ERROR,ENDFILE,
    ARRAY,INT,CHAR, 
    RETURN,IF,THEN,ELSE,ENDIF,WHILE,PROG,FUNC,READ,PRINT,BEGIN,END,LET,
    ID,NUM,ASSIGN,
    NOTEQUAL,EQUAL,LESS,GREATER,LESSEQ,GREATEREQ,
    NOT,PLUS,MINUS,MULT,DIVD,
    LPAREN,RPAREN,QUOTE,SEMI,COMA,LSQBRACKET,RSQBRACKET, SQUOTE
};

string tokenString[] = {
    "NONE","ERROR","ENDFILE",
    "ARRAY","INT","CHAR", 
    "RETURN", "IF","THEN","ELSE","ENDIF","WHILE", "PROG", "FUNC", "READ", "PRINT","BEGIN","END","LET",
    "ID", "NUM","ASSIGN", 
    "NOTEQUAL", "EQUAL", "LESS", "GREATER", "LESSEQ", "GREATEREQ",
    "NOT", "PLUS", "MINUS", "MULT", "DIVD", 
    "LPAREN", "RPAREN", "QUOTE", "SEMI", "COMA", "LSQBRACKET","RSQBRACKET", "SQUOTE"
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
