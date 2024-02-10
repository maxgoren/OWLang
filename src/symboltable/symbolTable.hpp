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
#ifndef symbolTable_hpp
#define symbolTable_hpp
#include <iostream>
using namespace std;

//An AVL Tree with a twist: key collisions
//are "stacked" at the node for controlling scope.
//if the last entry is "popped", the node is tombstoned
//instead of deleting the node, as there is a good 
//chance it may be used again. This way we avoid costly
//resturcturing/rebalancing.
class SymbolTable {
    private:
        struct Entry {
            string varname;
            int address;
            Entry* next;
            Entry(string v = "", int a = 0, Entry* n = nullptr) {
                varname = v;
                address = a;
                next = n;
            }
        };
        struct stnode {
            Entry *entry;
            int ht;
            stnode* left;
            stnode* right;
            stnode(string v, int loc) {
                entry = new Entry(v, loc, nullptr);
                ht = 0;
                left = right = nullptr;
            }
        };
        using node = stnode;
        using link = node*;
        link root;
        int count;
        int height(link h) { return (h == nullptr) ? -1:(h->ht); }
        int updatetht(link h) { return 1 + max(height(h->left), height(h->right)); }
        link rotL(link h) { 
            link x = h->right; h->right = x->left; x->left = h; 
            x->ht = updatetht(x); h->ht = updatetht(h);
            return x; 
        }
        link rotR(link h) { 
            link x = h->left; h->left = x->right; x->right = h; 
            x->ht = updatetht(x); h->ht = updatetht(h);
            return x; 
        }
        link fixUp(link h) {
            h->ht = updatetht(h);
            if (height(h->left) > height(h->right) + 1) 
                h = rotR(h);
            if (height(h->right) > height(h->left) + 1) 
                h = rotL(h);
            return h;
        }
        link put(link h, string k, int l) {
            if (h == nullptr) {
                count++;
                return new node(k, l);
            }
            if (k == h->entry->varname) {
                if (h->entry->address < 0)
                    h->entry->address = l;
                else
                    h->entry = new Entry(k, l, h->entry);
                return h;
            }
            if (k < h->entry->varname) {
                h->left = put(h->left, k, l);
            } else {
                h->right = put(h->right, k, l);
            }
            return fixUp(h);
        }
        int get(link h, string v) {
            if (h == nullptr)
                return -1;
            if (v == h->entry->varname)
                return h->entry->address;
            if (v < h->entry->varname) return get(h->left, v);
            else return get(h->right, v);
        }
        void cleanup(link h) {
            if (h != nullptr) {
                cleanup(h->left);
                cleanup(h->right);
                Entry* t = h->entry;
                while (t != nullptr) {
                    Entry* x = t;
                    t = t->next;
                    delete x;
                }
                delete h;
            }
        }
        void visit(link x) {
            if (x != nullptr) {
                visit(x->left);
                for (Entry* et = x->entry; et != nullptr; et = et->next) {
                    cout<<"name: "<<et->varname<<", address: "<<et->address<<endl;
                }
                visit(x->right);
            }
        }
    public:
        SymbolTable() {
            root = nullptr;
            count = 0;
        }
        ~SymbolTable() {
            cleanup(root);
        }
        void insert(string varname, int location) {
            root = put(root, varname, location);
        }
        void popKey(string varname) {
            link x = root;
            while (x != nullptr) {
                if (x->entry->varname == varname) {
                    //tombstone deletion on single entries.
                    if (x->entry->next == nullptr) {
                        x->entry->address = -1337;
                        return;
                    }
                    Entry* t = x->entry;
                    x->entry = x->entry->next;
                    delete t;
                    return;
                }
                x = (varname < x->entry->varname) ? x->left:x->right;
            }
        }
        int find(string varname) {
            return get(root, varname);
        }
        int size() {
            return count;
        }
        void dumpTable() {
            visit(root);
        }
        bool empty() {
            return root == nullptr;
        }
};


#endif
