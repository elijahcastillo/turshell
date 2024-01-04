#pragma once
#include "Lexer.h"
#include <string>
#include <vector>


/* =========== TYPES =========== */
/* class Visitor; */

struct ASTNode {
    // Base class for all AST nodes. All node types in the AST will inherit from this class.
    virtual ~ASTNode() = default;
    virtual void accept(class Visitor &v) = 0;  // Accept a visitor
};

struct ExpressionNode : public ASTNode {
    // Represents an expression. Expressions can be literals, identifiers, or compound expressions (like binary operations).
};

struct StatementNode : public ASTNode {
    // Represents a statement. A statement is an executable unit of code, like a variable declaration or a function call.
};

struct ProgramNode : public ASTNode {
  std::vector<ASTNode*> statements;

  ProgramNode(std::vector<ASTNode*>& stmts): statements(stmts) {};

    void accept(Visitor &v) override; 
};

struct BinaryExpressionNode : public ExpressionNode {
    // Represents a binary expression, like addition or subtraction.
    ASTNode* left;
    ASTNode* right;
    std::string op;

    BinaryExpressionNode(ASTNode* left, std::string& op, ASTNode* right)
        : left(left), right(right), op(op) {}

    void accept(Visitor &v) override; 
};


struct LogicalOperatorNode : public ExpressionNode {
    // Represents a binary expression, like addition or subtraction.
    ASTNode* left;
    ASTNode* right;
    std::string op;

    LogicalOperatorNode(ASTNode* left, std::string& op, ASTNode* right)
        : left(left), right(right), op(op) {}

    void accept(Visitor &v) override; 
};


struct UnaryExpressionNode : public ASTNode {
    std::string op;
    ASTNode* right;

    UnaryExpressionNode(std::string op, ASTNode* right) : op(op), right(right) {}


    void accept(Visitor &v) override;

    // Implement other necessary methods and destructor
};


struct IntLiteralNode : public ExpressionNode {
    int value;

    IntLiteralNode(const int value) : value(value) {}

    void accept(Visitor &v) override;
};

struct StringLiteralNode : public ExpressionNode {
    std::string value;

    StringLiteralNode(const std::string& value) : value(value) {}

    void accept(Visitor &v) override;
};

struct BinaryLiteralNode: public ExpressionNode {
    bool value;

    BinaryLiteralNode(bool value) : value(value) {}

    void accept(Visitor &v) override;
};


struct StructDeclarationNode: public ExpressionNode {
  std::string structName;
  std::vector<ASTNode*> properties;

  StructDeclarationNode(std::string& structName, std::vector<ASTNode*> properties) : structName(structName), properties(properties) {}

    void accept(Visitor &v) override;
};

struct StructInitalizerListNode : public ExpressionNode {
  std::vector<ASTNode*> properties; //Variable Assignment Nodes

  StructInitalizerListNode(std::vector<ASTNode*>& properties): properties(properties) {};

  void accept(Visitor &v) override;
};


struct StructPropertyAccessNode : public ExpressionNode {
    std::string baseName;
    std::vector<std::string> propertyNames;

    StructPropertyAccessNode(std::string& baseName, std::vector<std::string> propertyNames): baseName(baseName), propertyNames(propertyNames) {};

  void accept(Visitor &v) override;
};



//Ex: int a = 3 + 2;
struct VariableDeclarationNode : public StatementNode {
    std::string variableName;
    std::string variableType;
    ASTNode* initializer;

    VariableDeclarationNode(std::string& name, std::string& type, ASTNode* init): variableName(name), variableType(type), initializer(init){};

    void accept(Visitor &v) override; 
    // Constructor and accept method...
};


//Ex: a = 5;
struct VariableAssignmentNode : public ExpressionNode {
    std::string variableName;
    ASTNode* value;

    VariableAssignmentNode(std::string& name, ASTNode* value): variableName(name), value(value) {};

    void accept(Visitor &v) override; 
    // Constructor and accept method...
};


struct WhileStatementNode : public StatementNode {
    ASTNode* condition;
    ASTNode* body;

    WhileStatementNode(ASTNode* condition, ASTNode* body): condition(condition), body(body) {};


    void accept(Visitor &v) override; 
    // Constructor and accept method...
};

struct IfStatementNode : public StatementNode {
    ASTNode* condition;
    ASTNode* thenBranch;
    ASTNode* elseBranch;

    IfStatementNode(ASTNode* condition, ASTNode* thenBranch, ASTNode* elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    void accept(Visitor &v) override; 
};



struct FunctionCallNode : public ExpressionNode {
    std::string functionName;
    std::vector<ASTNode*> arguments;

    FunctionCallNode(const std::string& functionName, const std::vector<ASTNode*>& arguments)
        : functionName(functionName), arguments(arguments) {}

    void accept(Visitor &v) override;
};

//Node to hold a single parameter in a function
struct ParameterNode : public ASTNode {
    std::string type;
    std::string name;

    ParameterNode(const std::string& type, const std::string& name)
        : type(type), name(name) {}

    void accept(Visitor &v) override;
};

struct FunctionDeclarationNode : public StatementNode {
    std::string functionName;
    std::string returnType;
    std::vector<ASTNode*> parameters;
    ASTNode* body;

    FunctionDeclarationNode(const std::string& returnType, const std::string& functionName,
                            const std::vector<ASTNode*>& parameters, ASTNode* body)
        : returnType(returnType), functionName(functionName), parameters(parameters), body(body) {}

    void accept(Visitor &v) override;
};


//Code inside of {} like in a for, if, while, or function
struct BlockNode : public ASTNode {
    std::vector<ASTNode*> statements;

    BlockNode(const std::vector<ASTNode*>& stmts) : statements(stmts) {}

    void accept(Visitor &v) override;
};


//When using a variable, no delcaration or assignment
//Ex: if(a == 3)   a would be the VariableExpressionNode
struct VariableExpressionNode : public ExpressionNode {
    std::string variableName;

    VariableExpressionNode(const std::string& name) : variableName(name) {}

    void accept(Visitor &v) override; 
};

// return 2 + 3;
struct ReturnStatementNode : public StatementNode {
    ASTNode* expression; // The expression being returned, if any

    ReturnStatementNode(ASTNode* expression = nullptr)
        : expression(expression) {}

    void accept(Visitor &v) override;
};
