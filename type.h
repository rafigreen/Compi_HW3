#ifndef COMPI_HW3_TYPE_H
#define COMPI_HW3_TYPE_H

#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;

class Node {
public:
    string value;

    Node(const string value = "") : value(value) {
    };
    Node(const Node &node): value(node.value){
    };
    virtual ~Node() = default;
};

#define YYSTYPE Node*

class Funcs : public Node {
    virtual ~Funcs() = default;
};


class Type : public Node {
public:
    string type;

    Type(const string type);

    virtual ~Type() = default;
};

class RetType : public Node {
public:
    string type;

    RetType(const string type);

    virtual ~RetType() = default;
};

class FormalDecl : public Node {
public:
    string type;

    FormalDecl(Type *type, Node *node);
    FormalDecl(FormalDecl* formal);

    virtual ~FormalDecl(){
    };
};

class FormalsList : public Node {
public:
    vector<FormalDecl *> formals_list;

    FormalsList(Node *node);

    FormalsList(Node *node, FormalsList *formals_list);

    virtual ~FormalsList() {
        for (auto it = formals_list.begin(); it != formals_list.end(); it++)
            delete[] (*it);
    }
};

class Formals : public Node {
public:
    vector<FormalDecl *> formals_list;

    Formals();

    Formals(FormalsList *formals_list);

    virtual ~Formals() {
        for (auto it = formals_list.begin(); it != formals_list.end(); it++)
            delete[] (*it);
    }
};


class FuncDecl : public Node {
public:
    FuncDecl(RetType *return_type, Node *id, Formals *params);

    virtual ~FuncDecl() = default;
};


class Exp : public Node {
public:
    string type;
    string value;
    bool is_var=false;

    Exp() : type("void"), value("") {}

    Exp(Node *terminal, string type);

    Exp(Node *terminal1, Node *terminal2, const string op, const string type);

    Exp(bool is_var, Node *terminal);

    Exp(Node *exp, Node *type);

    Exp(Exp *exp);

    virtual ~Exp() = default;
};

class ExpList : public Node {
public:
    vector<Exp*> expressions;

    ExpList(Node *exp);

    ExpList(Node *exp_list, Node *exp);

    virtual ~ExpList() {
        for (auto it = expressions.begin(); it != expressions.end(); it++)
            delete[] &(*it);
    }
};


class Call : public Node {
public:
    string type;

    Call(Node *terminal);

    Call(Node *terminal, Node *exp_list);

    virtual ~Call() = default;
};

class Statement : public Node {
public:
    Statement(Node *node);

    Statement(Type *type, Node *id);

    Statement(Type *type, Node *id, Exp *exp);

    Statement(Node *id, Exp *exp);

    Statement( const string name, Exp *exp);

    Statement(Exp *exp, bool is_return=false);

    Statement(Call *call);

    virtual ~Statement() = default;
};

class Statements : public Node {
public:
    Statements(Statement *statement) : Node() {};

    Statements(Statements *statements, Statement *statement) : Node() {};

    virtual ~Statements() = default;
};

class Program : public Node {
public:
    Program();

    virtual ~Program() = default;
};

void check_bool(Node* node);

#endif
