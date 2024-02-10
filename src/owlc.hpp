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

#ifndef owlcompiler_hpp
#define owlcompiler_hpp
#include <iostream>
#include <fstream>
#include <vector>
#include "globals/tokens.hpp"
#include "backend/codegen.hpp"
#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"
#include "ast/syntaxTree.hpp"
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
        vector<Instruction> compile(string codeFileName, string asmFilename, bool loud) {
            ifstream infile;
            vector<string> codeVec;
            infile.open(codeFileName);
            if (infile.is_open()) {
                string line;
                while (getline(infile, line)) {
                    codeVec.push_back(line);
                }
                infile.close();
                return compile(codeVec, asmFilename, loud);
            }
            return {};
        }
        vector<Instruction> compile(vector<string>& codeVec, string filename, bool loud) {
            if (loud) {
                cout<<"------------------------------\n";
                cout<<"Phase 1: Lex & Parse."<<endl;
                cout<<"------------------------------\n";
            }
            ts = lexer.tokenize(codeVec);
            if (loud) printTokenStream(ts);
            ast = parser.parse(ts, loud);
            if (loud) printTree(ast);

            if (loud) {
                cout<<"------------------------------\n";
                cout<<"Phase 2: Code Generation."<<endl;
                cout<<"------------------------------\n";
            }

            vector<Instruction> owl = generatePCodeFromAST(ast, loud);

            ofstream outfile;
            outfile.open(filename);
            if (outfile.is_open()) {
                for (Instruction line : owl) {
                        line.output(outfile);
                        if (line.op == HLT)
                            break;
                }
                outfile.close();
            }
            cout<<"Compilation complete."<<endl;
            return owl;
        }
};


#endif
