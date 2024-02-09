#ifndef owlcompiler_hpp
#define owlcompiler_hpp
#include <iostream>
#include <fstream>
#include <vector>
#include "tokens.hpp"
#include "codegen.hpp"
#include "lexer.hpp"
#include "owlmachine.hpp"
#include "parser.hpp"
#include "syntaxTree.hpp"
using namespace std;


class OwlCompiler {
    private:
        OwlLexer lexer;
        OwlParser parser;
        TokenStream ts;
        SyntaxNode* ast;
    public:
        OwlCompiler() {

        }
        void compile(string codeFileName, string asmFilename, bool loud) {
            ifstream infile;
            vector<string> codeVec;
            infile.open(codeFileName);
            if (infile.is_open()) {
                string line;
                while (getline(infile, line)) {
                    codeVec.push_back(line);
                }
                infile.close();
                compile(codeVec, asmFilename, loud);
            }
        }
        void compile(vector<string>& codeVec, string filename, bool loud) {
            if (loud) {
                cout<<"\n\n------------------------------\n";
                cout<<"Phase 1: Lex & Parse."<<endl;
            }
            ts = lexer.tokenize(codeVec);
            if (loud) printTokenStream(ts);
            ast = parser.parse(ts, loud);
            if (loud) printTree(ast);

            if (loud) {
                cout<<"\n\n------------------------------\n";
                cout<<"Phase 2: Code Generation."<<endl;
            }

            vector<string> owl = generatePCodeFromAST(ast, loud);

            ofstream outfile;
            outfile.open(filename);
            if (outfile.is_open()) {
                for (string line : owl) {
                    if (line.size() > 0)
                        outfile << line << '\n';
                }
                outfile.close();
            }
            cout<<"Compilation complete."<<endl;
        }
};


#endif