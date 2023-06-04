#ifndef COMPI_HW3_SRC_H
#define COMPI_HW3_SRC_H

#include <string>
#include <vector>
#include <iostream>
#define DEBUG false

using std::string;
using std::vector;


string convert_to_upper_case(const string &str);

class Symbol {
public:
    string name;
    // If class is function, this is return type
    string type;
    int offset;
    bool is_function;
    // param types
    vector<string> params;
    bool is_overriden;

    Symbol(const string name, const string type, int offset,
           bool is_function, vector<string> params, bool is_overriden) : name(name),
                                                                         type(type),
                                                                         offset(offset),
                                                                         is_function(is_function),
                                                                        params(params),
                                                                        is_overriden(is_overriden) {}

    ~Symbol() = default;
};

class SymbolTable {

public:
    vector<Symbol*> symbols;
    int max_offset;
    bool is_loop;
    string* return_type;

    SymbolTable(int offset, bool is_loop, string return_type = "")
            : symbols(), max_offset(offset), is_loop(is_loop) {
        this->return_type = new string(return_type);
    }

    void add_symbol(const Symbol &symbol);

    bool symbol_exists(const string &name, bool *is_func);

    bool symbol_is_func(const string &name);
    
    bool symbol_overriden(const string &name, bool *exists);

    Symbol *get_symbol(const string &name);

    ~SymbolTable(){
        delete return_type;
        for(auto it = symbols.begin(); it != symbols.end(); it++)
            delete (*it);
    }
};

class TableStack {
public:
    vector<SymbolTable*> table_stack;
    vector<int> offsets;

    TableStack();

    void push_scope(bool is_loop = false, string return_type = "");

    SymbolTable *current_scope();

    void pop_scope();

    void add_symbol(const string &name, const string &type, bool is_function, 
                    vector<string> params = vector<string>(), bool is_overriden = false);

//!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!
//ADD HERE CODE FOR OVERRIDE!!!!!
//!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!! MAYBE!!!!!

    void add_function_symbol(const string &name, const string &type, int offset);

    bool symbol_exists(const string &name, bool *is_func);

    bool symbol_overriden(const string &name, bool *exists);

    bool check_loop();

    Symbol *get_symbol(const string &name);

    Symbol *get_overridden_symbol(const string &name, vector<string> exp_list);

    int get_num_overrides(const string &name);

    void get_override_types(const string &name, vector<string> &override_types);

    bool same_overriden_func_exists(const string &name, vector<string> &function_param_types);

    int num_compatible_func(const string &name, vector<string> &function_param_types);

    void insert_symbol(SymbolTable &table, Symbol &symbol);

    void print_scopes();

    void check_program();
    ~TableStack(){
        for(auto it = table_stack.begin(); it != table_stack.end(); it++){
            SymbolTable* current = *it;
            delete current;
        }

    }
};

#endif
