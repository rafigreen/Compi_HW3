#include <stack>
#include <map>
#include <memory>
#include <utility>
#include "Type.h"

#define GARBAGE -1

enum reloptype {
    EQUALS,
    NOT_EQUALS,
    SMALLER,
    BIGGER,
    SMALLER_EQUALS,
    BIGGER_EQUALS
};
///////////////////////////////////////////////////////////////////////////////////////////
enum binoptype {
    PLUS,
    MINUS,
    MUL,
    DIV
};
///////////////////////////////////////////////////////////////////////////////////////////
class Node {
public:
    int lineno;
    explicit Node(int num) : lineno(num) {}
};
///////////////////////////////////////////////////////////////////////////////////////////
class TypeNode : public Node {
public:
    basictype type;
    TypeNode(basictype t, int lineno) : Node(lineno), type(t) {}
};
///////////////////////////////////////////////////////////////////////////////////////////
class RetTypeNode : public Node {
public:
    basictype type;
    RetTypeNode(int lineno, basictype type) : Node(lineno), type(type) {}
};
///////////////////////////////////////////////////////////////////////////////////////////
class FormalsNode : public Node {
public:
    std::vector<Symbol> arguments;
    FormalsNode(int num, std::vector<Symbol> args);
    FormalsNode() : Node(GARBAGE) {}
    std::vector<Type> getArgumentsTypes();
};
///////////////////////////////////////////////////////////////////////////////////////////
class FormalsListNode : public Node {
public:
    std::vector<Symbol> args;
    explicit FormalsListNode(int num) : Node(num) {}
    FormalsListNode(int num, std::vector<Symbol> args) : Node(num), args(std::move(args)) {}
    void addArgument(int lineno, Symbol arg);
};
///////////////////////////////////////////////////////////////////////////////////////////
class FormalDeclNode : public Node {
public:
    Symbol arg;
    FormalDeclNode(int lineno, bool is_const, basictype type, std::string name);
};
///////////////////////////////////////////////////////////////////////////////////////////
class TypeAnnotationNode : public Node {
public:
    bool is_const;
    explicit TypeAnnotationNode(bool is_const, int lineno = GARBAGE) : Node(lineno), is_const(is_const) {}
};
///////////////////////////////////////////////////////////////////////////////////////////
class ExpNode : public Node {
public:
    basictype type;
    ExpNode(int lineno, basictype type) : Node(lineno), type(type) {}
};
///////////////////////////////////////////////////////////////////////////////////////////
class ExpListNode : public Node {
public:
    std::vector<Type> types;
    ExpListNode(int lineno) : Node(lineno) {}
    void addExp(basictype type);
};
///////////////////////////////////////////////////////////////////////////////////////////
class CallNode : public Node {
public:
    basictype type;
    CallNode(int lineno, basictype type) : Node(lineno), type(type) {}
};
///////////////////////////////////////////////////////////////////////////////////////////
class Relop : public Node {
public:
    enum reloptype type;
    Relop(std::string value, int num) : Node(num){
        if(value == "<") type = SMALLER;
        else if(value == ">") type = BIGGER;
        else if(value == ">=") type = BIGGER_EQUALS;
        else if(value == "<=") type = SMALLER_EQUALS;
        else if(value == "==") type = EQUALS;
        else if(value == "!=") type = NOT_EQUALS;
        else throw std::invalid_argument("wrong Relop");
    }
};
///////////////////////////////////////////////////////////////////////////////////////////
class Binop : public Node {
public:
    enum binoptype type;
    Binop(std::string value, int num) : Node(num) {
        if (value == "+") type = PLUS;
        else if (value == "-") type = MINUS;
        else if (value == "*") type = MUL;
        else if (value == "/") type = DIV;
        else throw std::invalid_argument("wrong Binop");
    }
};
///////////////////////////////////////////////////////////////////////////////////////////
class IdNode : public Node {
public:
    std::string name;
    IdNode(std::string text, int num) : Node(num), name(text) {}
};
///////////////////////////////////////////////////////////////////////////////////////////
class NumNode : public Node {
public:
    std::string value;
    NumNode(std::string text, int num) : Node(num), value(text) {}
    int getNumber() {
        return std::stoi(value);
    }
};
///////////////////////////////////////////////////////////////////////////////////////////
class StringNode : public Node {
public:
    std::string value;
    StringNode(std::string text, int num) : Node(num), value(text) {}
};

#define YYSTYPE Node*




//typedef std::stack<int> OffsetStack ;
//typedef std::map<std::string, Symbol> SymbolMap;
//typedef std::stack<SymbolMap> SymbolMapStack;
//extern SymbolMapStack symbolMapStack;
//extern OffsetStack offsetStack;



