#ifndef lexer_hpp
#define lexer_hpp
#include <iostream>
#include <vector>
#include "../globals/tokens.hpp"
using namespace std;

struct TokenStreamNode {
    Token token;
    TokenStreamNode* next;
    TokenStreamNode(Token token_) : token(token_), next(nullptr) { }
    TokenStreamNode() : next(nullptr) { }
};

class TokenStreamIter {
    private:
        TokenStreamNode* current;
    public:
        TokenStreamIter(TokenStreamNode* tsn = nullptr) {
            current = tsn;
        }
        Token& get() {
            return current->token;
        }
        Token peekRightOne() {
            if (current && current->next)
                return current->next->token;
            return get();
        }
        void advance() {
            current = current->next;
        }
        bool hasMore() {
            return current != nullptr;
        }
};

class TokenStream {
    private:
        TokenStreamNode* head;
        TokenStreamNode* tail;
        int nodeCount;
    public:
        TokenStream() {
            head = nullptr;
            tail = nullptr;
            nodeCount = 0;
        }
        bool empty() {
            return head == nullptr;
        }
        void addNode(TokenStreamNode* node) {
            if (empty()) {
                head = node;
                tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
            nodeCount++;
        }
        TokenStreamIter getStream() {
            return TokenStreamIter(head);
        }
        TokenStream concat(TokenStream& otherStream) {
            if (empty()) {
                head = otherStream.head;
                tail = otherStream.tail;
            } else {
                tail->next = otherStream.head;
                tail = otherStream.tail;
            }
            otherStream.head = nullptr;
            otherStream.tail = nullptr;
            otherStream.nodeCount = 0;
            return *this;
        }
};

void printTokenStream(TokenStream& ts) {
    for (auto it = ts.getStream(); it.hasMore(); it.advance()) {
        cout<<it.get().lineno<<":   <"<<tokenString[it.get().tokenval]<<", "<<it.get().stringval<<">"<<endl;
    }
}

class OwlLexer {
    private:
        TokenType handleKeywordOrId(string word);
        TokenStreamNode* handleSpecials(string word, int& i, int lno);
        int extractFullNumber(string line, int& i);
        string extractFullWord(string line, int& i);
        TokenStream tokenizeLine(string line, int lno);
    public:
        OwlLexer() {

        }
        TokenStream tokenize(vector<string>& program) {
            TokenStream tokenStream = tokenizeLine(program[0], 0);
            for (int i = 1; i < program.size(); i++) {
                TokenStream curr = tokenizeLine(program[i], i);
                tokenStream.concat(curr);
            }
            return tokenStream;
        }
};


TokenType OwlLexer::handleKeywordOrId(string word) {
    if (word == "begin") {
        return BEGIN;
    } else if (word == "end") {
        return END;
    } else if (word == "read") {
        return READ;
    } else if (word == "print") {
        return PRINT;
    } else if (word == "if") {
        return IF;
    } else if (word == "then") {
        return THEN;
    } else if (word == "else") {
        return ELSE;
    } else if (word == "endif") {
        return ENDIF;
    } else if (word == "func") {
        return FUNC;
    } else if (word == "while") {
        return WHILE;
    } else if (word == "let") {
        return LET;
    } else if (word == "int") {
        return INT;
    } else if (word == "char") {
        return CHAR;
    } else if (word == "program") {
        return PROG;
    } else if (word == "return") {
        return RETURN;
    } else if (word == "array") {
        return ARRAY;
    } else {
        return ID;
    }
}

TokenStreamNode* OwlLexer::handleSpecials(string line, int& i, int lno) {
    Token nextToken;
    nextToken.lineno = lno;
    if (line[i] == '(') {
        nextToken.tokenval = LPAREN;
        nextToken.stringval = "(";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == ')') {
        nextToken.tokenval = RPAREN;
        nextToken.stringval = ")";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '[') {
        nextToken.tokenval = LSQBRACKET;
        nextToken.stringval = "[";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == ']') {
        nextToken.tokenval = RSQBRACKET;
        nextToken.stringval = "]";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == ':' && i+1 < line.size() && line[i+1] == '=') {
        i+=2;
        nextToken.tokenval = ASSIGN;
        nextToken.stringval = ":=";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == ':') {
        nextToken.tokenval = COLON;
        nextToken.stringval = ":";
        nextToken.numval = -1;
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '!' && i+1 < line.size() && line[i+1] == '=') {
        i+=2;
        nextToken.tokenval = NOTEQUAL;
        nextToken.stringval = "!=";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '<' && i+1 < line.size() && line[i+1] == '=') {
        i+=2;
        nextToken.tokenval = LESSEQ;
        nextToken.stringval = "<=";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '>' && i+1 < line.size() && line[i+1] == '=') {
        i+=2;
        nextToken.tokenval = GREATEREQ;
        nextToken.stringval = ">=";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '+') {
        nextToken.tokenval = PLUS;
        nextToken.stringval = "+";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '-') {
        nextToken.tokenval = MINUS;
        nextToken.stringval = "-";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '*') {
        nextToken.tokenval = MULT;
        nextToken.stringval = "*";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '/') {
        nextToken.tokenval = DIVD;
        nextToken.stringval = "/";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '=') {
        nextToken.tokenval = EQUAL;
        nextToken.stringval = "=";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '<') {
        nextToken.tokenval = LESS;
        nextToken.stringval = "<";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '!') {
        nextToken.tokenval = NOT;
        nextToken.stringval = "!";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '\"') {
        nextToken.tokenval = QUOTE;
        nextToken.stringval = "\"";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == ';') {
        nextToken.tokenval = SEMI;
        nextToken.stringval = ";";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == ',') {
        nextToken.tokenval = COMA;
        nextToken.stringval = ",";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else if (line[i] == '\'') {
        nextToken.tokenval = SQUOTE;
        nextToken.stringval = "\'";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    } else {
        cout<<"ERROR\n";
        nextToken.tokenval = ERROR;
        nextToken.stringval = "ERROR";
        nextToken.numval = -1;    
        return new TokenStreamNode(nextToken);
    }
}

int OwlLexer::extractFullNumber(string line, int& i) {
    string asStr;
    while (isdigit(line[i]))
        asStr.push_back(line[i++]);
    i--;
    return atoi(asStr.c_str());
}

string OwlLexer::extractFullWord(string line, int& i) {
    string word;
    while (isalpha(line[i])) 
        word.push_back(line[i++]);
    i--;
    return word;
}

TokenStream OwlLexer::tokenizeLine(string line, int lno) {
    TokenStream tokenStream;
    for (int i = 0; i < line.size(); i++) {
        Token nextToken;
        nextToken.lineno = lno;
        if (isalpha(line[i])) {
            string word = extractFullWord(line, i);
            nextToken.tokenval = handleKeywordOrId(word);
            nextToken.stringval = word;
            tokenStream.addNode(new TokenStreamNode(nextToken));
            continue;
        } else if (isdigit(line[i])) {
                int num = extractFullNumber(line, i);
                nextToken.tokenval = NUM;
                nextToken.numval = num;
                nextToken.stringval = to_string(num);
                tokenStream.addNode(new TokenStreamNode(nextToken));
                continue;
        } else {
            if (line[i] != ' ') {
                tokenStream.addNode(handleSpecials(line, i, lno));
            }
        }
    }
    return tokenStream;
}

#endif
