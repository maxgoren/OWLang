#ifndef symbolTable_hpp
#define symbolTable_hpp
#include <iostream>
using namespace std;

//Simple AVLTree based symbol table used for associating 
//variable names to memeory addresses during compilation
class SymbolTable {
    private:
        struct Entry {
            string varname;
            int address;
            int ht;
            Entry* left;
            Entry* right;
            Entry(string v, int loc) {
                varname = v; address = loc; ht = 0;
                left = right = nullptr;
            }
        };
        using node = Entry;
        using link = node*;
        link root;
        int count;
        int height(Entry* h) { return (h == nullptr) ? -1:(h->ht); }
        int updatetht(Entry* h) { return 1 + max(height(h->left), height(h->right)); }
        link rotL(link h) { 
            Entry* x = h->right; h->right = x->left; x->left = h; 
            x->ht = updatetht(x); h->ht = updatetht(h);
            return x; 
        }
        link rotR(link h) { 
            Entry* x = h->left; h->left = x->right; x->right = h; 
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
            if (k == h->varname)
                return h;
            if (k < h->varname) {
                h->left = put(h->left, k, l);
            } else {
                h->right = put(h->right, k, l);
            }
            return fixUp(h);
        }
        int get(link h, string v) {
            if (h == nullptr)
                return -1;
            if (v == h->varname)
                return h->address;
            if (v < h->varname) return get(h->left, v);
            else return get(h->right, v);
        }
        void cleanup(link h) {
            if (h != nullptr) {
                cleanup(h->left);
                cleanup(h->right);
                delete h;
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
        int find(string varname) {
            return get(root, varname);
        }
        int size() {
            return count;
        }
        bool empty() {
            return root == nullptr;
        }
};


#endif
