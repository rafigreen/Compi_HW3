#include "type.h"
#include "src.h"
#include "hw3_output.hpp"

#define TRUE "true"
#define FALSE "false"
extern TableStack tables;
extern int yylineno;

void this_is_a_test()
{
    //std::cout << "TESTING HERE"<< std::endl;
    int x = 5;
    int y = x - 5;
    if(y != 0)
        return;
    else
    {
        return;
    }
}

static bool check_types_compatible(string type1, string type2) {
    if (type1 == type2)
        return true;
    this_is_a_test();
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
        this_is_a_test();


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
OverrideDecl::OverrideDecl(bool overriden):Node(), is_overriden(overriden)
{}

FuncDecl::FuncDecl(RetType *return_type, Node *id, Formals *params, OverrideDecl *is_overriden) {
    if (DEBUG)
        std::cout << "FuncDecl " << std::endl;
    bool overriden = is_overriden->is_overriden;
    bool is_func = false;
    bool sym_exists = tables.symbol_exists(id->value, &is_func);

    if(id->value == "main" && is_overriden->is_overriden)
    {
        output::errorMainOverride(yylineno);
        exit(0);
    }

    if (sym_exists && !is_func)
    {
        output::errorDef(yylineno, id->value);
        exit(0);
    }

    //RAFI ADDED THIS (it means a symbol already exists)
/////////////////////////////////////
    bool func_exists = false;
    bool sym_overriden = tables.symbol_overriden(id->value, &func_exists);
    int over_count = tables.get_num_overrides(id->value);

    vector<string> function_param_types = vector<string>();
    for (int i = 0; i < params->formals_list.size(); ++i) 
    {
        auto it = params->formals_list[i];
        function_param_types.push_back((*it).type);
    }
 /////////////########////////////////  
 ////////////CHECK IF OVERRIDING SAME THING//////// 
    
    //SAME OVERRIDEN FUNC ALREADY EXISTS
    //   I.E. 
    //  override int foo(int x){return x;}
    //  override int foo(int x){return x;}
    //  => error
    if((overriden && sym_overriden))
    {
        if (tables.same_overriden_func_exists(id->value, function_param_types))
        {
            output::errorDef(yylineno, id->value);
            exit(0);
        }             
    }
    
/////////////########////////////////
//THIS WOULD MEAN IT EXISTS (errorDef) OR IT IS NOT OVERRIDEN

    if (func_exists && !sym_overriden && !overriden) {

        output::errorDef(yylineno, id->value);
        exit(0);
    }

    if(func_exists && !sym_overriden && overriden) {

        output::errorFuncNoOverride(yylineno, id->value);
        exit(0);
    }

    if(func_exists && sym_overriden && !overriden) {

        output::errorOverrideWithoutDeclaration(yylineno, id->value);
        exit(0);
    }
/////////////CHECK HERE IF THERE IS A SEPERATE DATABASE FOR FUNCTIONS
    tables.add_symbol(id->value, return_type->type, true, function_param_types, overriden);
    tables.push_scope(false, return_type->type);
    bool dummy;
    int offset = -1;
    for (int i = 0; i < params->formals_list.size(); ++i) {
        auto it = params->formals_list[i];
        if (tables.symbol_exists(it->value, &dummy)) {
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
        this_is_a_test();

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
    this_is_a_test();

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
    bool dummy;
    if (tables.symbol_exists(id->value, &dummy)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    tables.add_symbol(id->value, type->type, false);
    value = type->value;//

}
//
//
//NEED TO ADD A VECTOR OF TYPES FOR OVERRIDEN TYPES
//
//

// Statement -> Type ID ASSIGN Exp SC or Statement -> AUTO ID ASSIGN Exp SC
Statement::Statement(Type *type, Node *id, Exp *exp) : Node() {
    if (DEBUG)
        std::cout << "Statement -> Type ID ASSIGN Exp SC\n";
    bool dummy;
    this_is_a_test();
    if (tables.symbol_exists(id->value, &dummy)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }

    bool is_overriden = tables.symbol_overriden(exp->value, &dummy);

    if (type && !is_overriden) {
        if (!check_types_compatible(type->type, exp->type)) {
            //std::cout << "WAKA1";
            output::errorMismatch(yylineno);
            exit(0);
        }
        if (type->type == "byte" && exp->type == "int") {
           // std::cout << "WAKA2";
            output::errorMismatch(yylineno);
            exit(0);
        }
        tables.add_symbol(id->value, type->type, false);
    }
    else if(type && is_overriden)
    {
        vector<string> over_types = vector<string>();
        tables.get_override_types(exp->value, over_types);
        int i;
        //std::cout << over_types.size();
        for(i = 0; i < over_types.size(); i++)
        {
            //std::cout << over_types[i] << std::endl;
            //std::cout <<"WAKA 4" << std::endl;
            if((over_types[i] == exp->type) 
                || (over_types[i] == "int" && exp->type == "byte"))
            {
                i = -1;
                break;
            }
        }
        //this means there are no overriden types that were called
        if(i != -1)
        {
            output::errorMismatch(yylineno);
            exit(0);
        }
        over_types.clear();
        tables.add_symbol(id->value, type->type, false);
    } 
    
    else 
    {
        if (exp->type == "void" || exp->type == "string") {
            //std::cout << "WAKA3";
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
    bool dummy;
    if (!tables.symbol_exists(id->value, &dummy)) {
        output::errorUndef(yylineno, id->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(id->value);
    if (symbol->is_function || !check_types_compatible(symbol->type, exp->type)) {
       // std::cout << "WAKA4";
        this_is_a_test();
        output::errorMismatch(yylineno);
        exit(0);
    }
    if(symbol->type == "byte" && exp->type == "int"){
        //std::cout << "WAKA5";
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
    bool dummy;
    if (!tables.symbol_exists(call->value, &dummy)) {
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
    this_is_a_test();
    if (DEBUG)
        std::cout << "Exp (bool)\n";
    if (exp->type != "bool") {
       // std::cout << "WAKA6";
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
Exp::Exp(Exp *exp) : Node(exp->value), type(exp->type), overriden_types() {
}

// Exp -> CONST(bool, num, byte, string)
Exp::Exp(Node *terminal, string
type) : Node(terminal->value), type(type), overriden_types() {
    this_is_a_test();
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

////////////////////////////////////////
////////ADD TYPES OF OVERRIDEN FUNCS FOR FUTURE CHECK
///////////////////////////////////////

Exp::Exp(bool is_var, Node *terminal) : Node(), is_var(is_var), overriden_types() {
    this_is_a_test();
    if (DEBUG)
        std::cout << "Exp -> ID, Call " << terminal->value << " is var: " << is_var << std::endl;
    bool dummy;
    if (is_var && !tables.symbol_exists(terminal->value, &dummy)) {
        output::errorUndef(yylineno, terminal->value);
        exit(0);
    }
    bool func_exists = false;
    bool sym_overriden = tables.symbol_overriden(terminal->value, &func_exists);
    int over_count = tables.get_num_overrides(terminal->value);
    if(over_count > 1)
    {
        tables.get_override_types(terminal->value, this->overriden_types);////!!!!!///!!!!///
    }
    Symbol *symbol = tables.get_symbol(terminal->value);
    value = terminal->value;
    type = symbol->type;
}

Exp::Exp(Node *terminal1, Node *terminal2,
         const string op,
         const string type) : overriden_types() {
            this_is_a_test();
    Exp *exp1 = dynamic_cast<Exp *>(terminal1);
    Exp *exp2 = dynamic_cast<Exp *>(terminal2);

    if (DEBUG)
        std::cout << "Exp op " << exp1->type << " " << exp1->value << " " << exp2->type << " " << exp2->value
                  << std::endl;
    bool dummy;
    if (exp1->is_var && !tables.symbol_exists(exp1->value, &dummy)) {
        output::errorUndef(yylineno, terminal1->value);
        exit(0);
    }
    
    if (exp2->is_var && !tables.symbol_exists(exp2->value, &dummy)) {
        output::errorUndef(yylineno, terminal2->value);
        exit(0);
    }

    if (!check_types_compatible(exp1->type, exp2->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    if (type == "bool") {
        this_is_a_test();
//        bool t1, t2, res;
        if (exp1->type != "bool" || exp2->type != "bool") {
            output::errorMismatch(yylineno);
            exit(0);
        }

        this->type = "bool";
    } else if (type == "int") {

        if ((exp1->type != "int" && exp1->type != "byte") || (exp1->type != "int" && exp1->type != "byte")) {
            this_is_a_test();
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
        this_is_a_test();
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
Exp::Exp(Node *exp, Node *type) : overriden_types(){
    this_is_a_test();
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
    this_is_a_test();
    if (DEBUG)
        std::cout << "ExpList -> Exp: " << exp->value << "\n";
    Exp *expression = dynamic_cast<Exp *>(exp);
    expressions.push_back(expression);
//    delete[] exp;
}

// ExpList -> Exp, ExpList
ExpList::ExpList(Node *exp_list, Node *exp) : Node(), expressions() {
    this_is_a_test();
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
    this_is_a_test();
    if (DEBUG)
        std::cout << "Call " << terminal->value << std::endl;

    bool func_exists = false;
    bool sym_overriden = tables.symbol_overriden(terminal->value, &func_exists);
    int over_count = tables.get_num_overrides(terminal->value);
    string name = terminal->value;
    bool dummy;
    if (!tables.symbol_exists(name, &dummy)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }

    Symbol *symbol = tables.get_symbol(name);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
    /*if(sym_overriden && over_count > 1)
    {
        output::errorAmbiguousCall(yylineno, name);
        exit(0);
    }*/
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//CHANGE HERE FOR OVERRIDEN FROM NO ARGUMENTS TO SOME ARGUMENTS
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (!sym_overriden && symbol->params.size() > 0) {
        vector<string> converted_params;
        for (int i = 0; i < symbol->params.size(); ++i) {
            converted_params.push_back(convert_to_upper_case(symbol->params[i]));
        }
        output::errorPrototypeMismatch(yylineno, name);
        exit(0);
    }



    type = symbol->type;
    value = symbol->name;
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//CHANGE HERE FOR OVERRIDEN FROM NO ARGUMENTS TO SOME ARGUMENTS
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Call -> ID LPAREN ExpList RPAREN

//NEED TO CHECK IF AN EXPRESSION IS A FUNCTION CALL
Call::Call(Node *terminal, Node *exp_list) : Node() {
    if (DEBUG)
        std::cout << "Call " << terminal->value << std::endl;
    
    bool func_exists = false;
    bool sym_overriden = tables.symbol_overriden(terminal->value, &func_exists);
    int over_count = tables.get_num_overrides(terminal->value);

    ExpList *expressions_list = dynamic_cast<ExpList *>(exp_list);
    string name = terminal->value;
//    std::cout << "WAKA";
    bool dummy;
    if (!tables.symbol_exists(name, &dummy)) {
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
    if (!sym_overriden && symbol->params.size() != expressions_list->expressions.size()) {
        //std::cout << "WAKA3";
        output::errorPrototypeMismatch(yylineno, name);
        exit(0);
    }
//    
    vector<string> sym_params = vector<string>();
    vector<string> over_types = vector<string>();
    for (int j = 0; j < expressions_list->expressions.size(); ++j) {
        sym_params.push_back(expressions_list->expressions[j]->type);
        //std::cout << expressions_list->expressions[j]->type << std::endl;
    }
   // Symbol *over_symbol = tables.get_overridden_symbol(name, sym_params);

    bool exists;
    

    for (int j = 0; j < expressions_list->expressions.size(); ++j)
    {
        if(tables.symbol_overriden(expressions_list->expressions[j]->value, &exists))
        {
            tables.get_override_types(expressions_list->expressions[j]->value, over_types);
            int i;
            //std::cout << over_types.size();
            for(i = 0; i < over_types.size(); i++)
            {
                //std::cout << over_types[i] << std::endl;
                //std::cout <<"WAKA 4" << std::endl;
                if((over_types[i] == expressions_list->expressions[j]->type) 
                   || (expressions_list->expressions[j]->type == "byte" && over_types[i] == "int"))
                {
                    i = -1;
                    break;
                }
            }
            //this means there are no overriden types that were called
            if(i != -1)
            {
                output::errorPrototypeMismatch(yylineno, name);
                exit(0);
            }
            over_types.clear();
        }
    }

if(!sym_overriden)
{
    for (int i = 0; i < symbol->params.size(); i++) {
        if (symbol->params[i] != expressions_list->expressions[i]->type) {
            if (symbol->params[i] != "int" || expressions_list->expressions[i]->type != "byte")
            {
                //std::cout << "WAKA5";
                output::errorPrototypeMismatch(yylineno, name);
                exit(0);
            }
        }
    } 
}

int num_compat = tables.num_compatible_func(name, sym_params);
if(num_compat > 1)
{
    output::errorAmbiguousCall(yylineno, name);
    exit(0);
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


