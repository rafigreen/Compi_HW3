#include "type.h"
#include "src.h"
#include "hw3_output.hpp"

#define TRUE "true"
#define FALSE "false"
extern TableStack tables;
extern int yylineno;

static bool check_types_compatible(string type1, string type2) {
    if (type1 == type2)
        return true;

    if (type1 == "bool" || type2 == "bool" || type1 == "string" || type2 == "string" || type2 == "void")
        return false;
    return true;
}

//************FORMALS******************
FormalDecl::FormalDecl(FormalDecl *formal) : Node(formal->value), type(formal->type) {
    if (DEBUG)
        std::cout << "FormalDecl " << this->type << " " << this->value << std::endl;
}

FormalDecl::FormalDecl(Type *type, Node *node) : Node(*node), type(type->type) {
    if (DEBUG)
        std::cout << "FormalDecl " << this->type << " " << this->value << std::endl;
//    delete[] node;
//    delete[] type;
}

FormalsList::FormalsList(Node *node) : Node(), formals_list() {
    if (DEBUG)
        std::cout << "FormalsList " << std::endl;
    formals_list.insert(formals_list.begin(), new FormalDecl(dynamic_cast<FormalDecl *>(node)));
//    delete[] node;
}

FormalsList::FormalsList(Node *node, FormalsList *formals_list) : Node(), formals_list() {
    if (DEBUG)
        std::cout << "FormalsList " << std::endl;
    for (auto it = formals_list->formals_list.begin(); it != formals_list->formals_list.end(); ++it) {
        FormalDecl *decl = new FormalDecl(*it);
        this->formals_list.push_back(decl);
    }
    this->formals_list.insert(this->formals_list.begin(), dynamic_cast<FormalDecl *>(node));
//    delete[] formals_list;
}

Formals::Formals() : Node(), formals_list() {
    if (DEBUG)
        std::cout << "Formals " << std::endl;

}

Formals::Formals(FormalsList *formals_list) : Node(), formals_list() {
    if (DEBUG)
        std::cout << "Formals " << std::endl;
    for (auto it = formals_list->formals_list.begin(); it != formals_list->formals_list.end(); ++it) {
        FormalDecl *decl = new FormalDecl(*it);
        this->formals_list.push_back(decl);
    }

//    delete[] formals_list;
}

//************FUNCDECL*****************
// FuncDecl -> RetType ID LPAREN Formals RPAREN LBRACE OS Statements ES RBRACE
//!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!
//ADD HERE CODE FOR OVERRIDE!!!!!
//!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!
FuncDecl::FuncDecl(RetType *return_type, Node *id, Formals *params, bool overriden) {
    if (DEBUG)
        std::cout << "FuncDecl " << std::endl;
    
    bool is_func = false;
    bool sym_exists = tables.symbol_exists(id->value, &is_func);
    if (sym_exists && !is_func)
    {
        output::errorDef(yylineno, id->value);
        exit(0);
    }

    //RAFI ADDED THIS (it means a symbol already exists)
/////////////////////////////////////
    bool func_exists = false;
    bool sym_overriden = tables.symbol_overriden(id->value, &func_exists);

    vector<string> function_param_types = vector<string>();
    for (int i = 0; i < params->formals_list.size(); ++i) 
    {
        auto it = params->formals_list[i];
        function_param_types.push_back((*it).type);
    }
 /////////////########////////////////  
 ////////////CHECK IF OVERRIDING SAME THING//////// 
    if((overriden && sym_overriden))
    {
        Symbol* sym = tables.get_symbol(id->value);
        if(params->formals_list.size() == sym->params.size())
        for (int i = 0; i < params->formals_list.size(); ++i) 
        {
            auto it_1 = params->formals_list[i];
            auto it_2 = sym->params[i];
            if((*it_1).type != it_2)
            {
                break;
            }
        }       
    }

    
/////////////########////////////////
//THIS WOULD MEAN IT EXISTS (errorDef) OR IT IS NOT OVERRIDEN

    if (func_exists && !sym_overriden) {

        output::errorDef(yylineno, id->value);
        exit(0);
    }


/////////////CHECK HERE IF THERE IS A SEPERATE DATABASE FOR FUNCTIONS
    tables.add_symbol(id->value, return_type->type, true, function_param_types, overriden);
    tables.push_scope(false, return_type->type);
    int offset = -1;
    for (int i = 0; i < params->formals_list.size(); ++i) {
        auto it = params->formals_list[i];
        if (tables.symbol_exists(it->value)) {
            output::errorDef(yylineno, it->value);
            exit(0);
        }
        tables.add_function_symbol(it->value, it->type, offset);
        offset -= 1;
    }
//    delete[] return_type;
//    delete[] id;
//    delete[] params;
}

//************STATEMENT****************
// Statement -> BREAK SC / CONTINUE SC
Statement::Statement(Node *node) {
    if (node->value == "break") {
        if (!tables.check_loop()) {
            output::errorUnexpectedBreak(yylineno);
            exit(0);
        }
    } else if (node->value == "continue") {
        if (!tables.check_loop()) {
            output::errorUnexpectedContinue(yylineno);
            exit(0);
        }
    }

//    delete[] node;

}

Statement::Statement(Exp *exp, bool is_return) : Node() {
    if (DEBUG)
        std::cout << "Statement Expression is_return:" << is_return << std::endl;
    SymbolTable *scope = tables.current_scope();
    string *return_type = scope->return_type;

    if (*return_type != "" && *return_type != exp->type) {
        if (*return_type != "int" || exp->type != "byte") {
            output::errorMismatch(yylineno);
            exit(0);
        }

    }
    if (exp->is_var) {
        Symbol *symbol = tables.get_symbol(exp->value);
        if (symbol->is_function) {
            output::errorUndef(yylineno, symbol->name);
            exit(0);
        }
    }
}

// Statement -> Type ID SC
Statement::Statement(Type *type, Node *id) : Node() {
    if (DEBUG)
        std::cout << "Statement Type ID: " << type->type << " " << id->value << std::endl;
    if (tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    tables.add_symbol(id->value, type->type, false);
    value = type->value;//

}

// Statement -> Type ID ASSIGN Exp SC or Statement -> AUTO ID ASSIGN Exp SC
Statement::Statement(Type *type, Node *id, Exp *exp) : Node() {
    if (DEBUG)
        std::cout << "Statement -> Type ID ASSIGN Exp SC\n";

    if (tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    if (type) {
        if (!check_types_compatible(type->type, exp->type)) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if (type->type == "byte" && exp->type == "int") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        tables.add_symbol(id->value, type->type, false);
    } else {
        if (exp->type == "void" || exp->type == "string") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        // For Bool expressions
//        if (!check_types_compatible(type->type, exp->type)) {
//            output::errorMismatch(yylineno);
//            exit(0);
//        }
        tables.add_symbol(id->value, exp->type, false);
    }

    // TODO: do something with value/type?
}

// Statement -> ID ASSIGN Exp SC
Statement::Statement(Node *id, Exp *exp) : Node() {
    if (!tables.symbol_exists(id->value)) {
        output::errorUndef(yylineno, id->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(id->value);
    if (symbol->is_function || !check_types_compatible(symbol->type, exp->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if(symbol->type == "byte" && exp->type == "int"){
        output::errorMismatch(yylineno);
        exit(0);
    }
    // TODO: do something with value/type?
}

//!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!
//ADD HERE CODE FOR OVERRIDE!!!!!
//!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!


// Statement -> Call SC
Statement::Statement(Call *call) : Node() {
    if (DEBUG)
        std::cout << "Statement Call " << call->value << std::endl;
    if (!tables.symbol_exists(call->value)) {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(call->value);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    // TODO: do something with value/type?
}

Statement::Statement(const string name, Exp *exp) {
    if (DEBUG)
        std::cout << "Exp (bool)\n";
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

//****************TYPE************************
Type::Type(
        const string type) : Node(), type(type) {

}

RetType::RetType(
        const string type) : Node(), type(type) {}

// ***************EXP******************
// Exp -> LPAREN Exp RPAREN
Exp::Exp(Exp *exp) : Node(exp->value), type(exp->type) {
}

// Exp -> CONST(bool, num, byte, string)
Exp::Exp(Node *terminal, string
type) : Node(terminal->value), type(type) {
    if (DEBUG)
        std::cout << "Exp Node+string " << type << " " << terminal->value << std::endl;
    if (type == "byte") {
        int value = stoi(terminal->value);
        if (value > 255) {
            output::errorByteTooLarge(yylineno, terminal->value);
            exit(0);
        }
    }
}

//Exp -> ID, Call
Exp::Exp(bool is_var, Node *terminal) : Node(), is_var(is_var) {
    if (DEBUG)
        std::cout << "Exp -> ID, Call " << terminal->value << " is var: " << is_var << std::endl;
    if (is_var && !tables.symbol_exists(terminal->value)) {
        output::errorUndef(yylineno, terminal->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(terminal->value);
    value = terminal->value;
    type = symbol->type;
}

Exp::Exp(Node *terminal1, Node *terminal2,
         const string op,
         const string type) {
    Exp *exp1 = dynamic_cast<Exp *>(terminal1);
    Exp *exp2 = dynamic_cast<Exp *>(terminal2);

    if (DEBUG)
        std::cout << "Exp op " << exp1->type << " " << exp1->value << " " << exp2->type << " " << exp2->value
                  << std::endl;
    if (exp1->is_var && !tables.symbol_exists(exp1->value)) {
        output::errorUndef(yylineno, terminal1->value);
        exit(0);
    }

    if (exp2->is_var && !tables.symbol_exists(exp2->value)) {
        output::errorUndef(yylineno, terminal2->value);
        exit(0);
    }

    if (!check_types_compatible(exp1->type, exp2->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    if (type == "bool") {
//        bool t1, t2, res;
        if (exp1->type != "bool" || exp2->type != "bool") {
            output::errorMismatch(yylineno);
            exit(0);
        }

        this->type = "bool";
    } else if (type == "int") {

        if ((exp1->type != "int" && exp1->type != "byte") || (exp1->type != "int" && exp1->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }

        if (exp1->type == "int" || exp2->type == "int") {
            this->type = "int";
        } else {
            this->type = "byte";
        }
        if (DEBUG)
            std::cout << "op return type is " << this->type << "\n";

    } else if (type == "relop") {
        if (!check_types_compatible(exp1->type, exp2->type)) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if ((exp1->type != "int" && exp1->type != "byte") || (exp1->type != "int" && exp1->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        this->type = "bool";
    }
}

// Exp -> LPAREN Type RPAREN Exp
Exp::Exp(Node *exp, Node *type) {
    Exp *converted_exp = dynamic_cast<Exp *>(exp);
    Type *converted_type = dynamic_cast<Type *>(type);

    if (!check_types_compatible(converted_exp->type, converted_type->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    this->value = converted_exp->value;
    this->type = converted_type->type;
}

//*******************EXPLIST************************

// ExpList -> Exp
ExpList::ExpList(Node *exp) : Node(), expressions() {
    if (DEBUG)
        std::cout << "ExpList -> Exp: " << exp->value << "\n";
    Exp *expression = dynamic_cast<Exp *>(exp);
    expressions.push_back(expression);
//    delete[] exp;
}

// ExpList -> Exp, ExpList
ExpList::ExpList(Node *exp_list, Node *exp) : Node(), expressions() {
    if (DEBUG)
        std::cout << "ExpList -> Exp,ExpList" << "\n";
    expressions.push_back(dynamic_cast<Exp *>(exp));
    vector<Exp *> expressions_list = (dynamic_cast<ExpList *>(exp_list))->expressions;
    for (int i = 0; i < expressions_list.size(); ++i) {
        expressions.push_back(new Exp(expressions_list[i]));

    }

//        delete[] exp;
//        delete[] exp_list;
}

//*******************CALL*********************

// Call -> ID LPAREN RPAREN
Call::Call(Node *terminal) : Node() {
    if (DEBUG)
        std::cout << "Call " << terminal->value << std::endl;

    string name = terminal->value;
    if (!tables.symbol_exists(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }

    Symbol *symbol = tables.get_symbol(name);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//CHANGE HERE FOR OVERRIDEN FROM NO ARGUMENTS TO SOME ARGUMENTS
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (symbol->params.size() > 0) {
        vector<string> converted_params;
        for (int i = 0; i < symbol->params.size(); ++i) {
            converted_params.push_back(convert_to_upper_case(symbol->params[i]));
        }
        output::errorPrototypeMismatch(yylineno, name, converted_params);
        exit(0);
    }

    type = symbol->type;
    value = symbol->name;
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//CHANGE HERE FOR OVERRIDEN FROM NO ARGUMENTS TO SOME ARGUMENTS
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Call -> ID LPAREN ExpList RPAREN
Call::Call(Node *terminal, Node *exp_list) : Node() {
    if (DEBUG)
        std::cout << "Call " << terminal->value << std::endl;
    ExpList *expressions_list = dynamic_cast<ExpList *>(exp_list);
    string name = terminal->value;
//    std::cout << "WAKA";
    if (!tables.symbol_exists(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
//    std::cout << "WAKA1";
    Symbol *symbol = tables.get_symbol(name);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
//HERE WE NEED TO CHANGE FOR OVERRIDE
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
//    std::cout << "WAKA2";
    if (symbol->params.size() != expressions_list->expressions.size()) {
        vector<string> converted_params;
        for (int i = 0; i < symbol->params.size(); ++i) {
            converted_params.push_back(convert_to_upper_case(symbol->params[i]));
        }
        output::errorPrototypeMismatch(yylineno, name, converted_params);
        exit(0);
    }
//    std::cout << "WAKA3";
    for (int i = 0; i < symbol->params.size(); i++) {
        if (symbol->params[i] != expressions_list->expressions[i]->type) {
            if (symbol->params[i] != "int" || expressions_list->expressions[i]->type != "byte") {
                vector<string> converted_params = vector<string>();
                for (int j = 0; j < symbol->params.size(); ++j) {
                    converted_params.push_back(convert_to_upper_case(symbol->params[j]));
                }
                output::errorPrototypeMismatch(yylineno, name, converted_params);
                exit(0);
            }
        }
    }
//    std::cout << "WAKA4";
    type = symbol->type;
    value = symbol->name;
}

Program::Program() {

}

void check_bool(Node *node) {
    Exp *exp = dynamic_cast<Exp *>(node);
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

