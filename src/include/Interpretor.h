
#pragma once
#include "AST_Types.h"
#include "Visitors.h"
#include "Enviorment.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <stack>

class Interpreter : public Visitor {
    // Environment to store variable values, function definitions, etc.

    std::stack<std::shared_ptr<Environment>> envStack; //Stack of Enviornment scopes
    std::stack<RuntimeVal> evaluationStack; //Stack to help durring calculations
    std::unordered_map<std::string, FunctionDeclarationNode*> functionTable; //User defined functions
    std::unordered_map<std::string, std::function<RuntimeVal(Interpreter&, std::vector<RuntimeVal>&)>> nativeFunctions;


    enum class Context {
        Expression,
        Statement
    };

    Context currentContext;



public:

    Interpreter(){
      envStack.push(std::make_shared<Environment>());
    }

    void registerNativeFunction(const std::string& name, std::function<RuntimeVal(Interpreter&, std::vector<RuntimeVal>&)> func); 
    RuntimeVal callNativeFunction(const std::string& name, std::vector<RuntimeVal>& args); 

    void visit(ProgramNode& node) override;

    void visit(BinaryExpressionNode& node) override;

    void visit(IntLiteralNode& node) override ;
    void visit(StringLiteralNode& node) override ;

    void visit(VariableDeclarationNode& node) override;

    void visit(VariableAssignmentNode& node) override;

    void visit(WhileStatementNode& node) override ;

    void visit(IfStatementNode& node) override;

    void visit(FunctionDeclarationNode& node) override ;

    void visit(BlockNode& node) override;

    void visit(VariableExpressionNode& node) override;

    void visit(ReturnStatementNode& node) override;

    void visit(ParameterNode& node) override;
    void visit(FunctionCallNode& node) override;


    std::shared_ptr<Environment> currentScope();

    void enterNewScope();

    void exitCurrentScope();
    // Helper methods
    RuntimeVal evaluateExpression(ASTNode* node); 

    // Error handling methods
    void runtimeError(const std::string& message);

    void printStack(); // Method to print the stack contents
    void printEnv(); // Method to print the stack contents
                       //

    // Add more helper and utility methods as needed...
};
