
#pragma once
#include "AST_Types.h"
#include "Visitors.h"
#include "Enviorment.h"
#include <memory>
#include <unordered_map>
#include <stack>

class Interpreter : public Visitor {
    // Environment to store variable values, function definitions, etc.

    std::stack<std::shared_ptr<Environment>> envStack;
    std::stack<RuntimeVal> evaluationStack;



public:

    Interpreter(){
      envStack.push(std::make_shared<Environment>());
    }

    void visit(ProgramNode& node) override;

    void visit(BinaryExpressionNode& node) override;

    void visit(IntLiteralNode& node) override ;

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

    void enterScope();

    void exitScope();
    // Helper methods
    RuntimeVal evaluateExpression(ASTNode* node); 

    // Error handling methods
    void runtimeError(const std::string& message);

    void printStack(); // Method to print the stack contents

    // Add more helper and utility methods as needed...
};
