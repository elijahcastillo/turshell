#pragma once
#include "AST_Types.h"
#include <iostream>

/* ========== Visitors ========== */


class BinaryExpressionNode;  // Forward declaration
class IntLiteralNode;        // Forward declaration

class Visitor {
public:
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(BinaryExpressionNode& node) = 0;
    virtual void visit(LogicalOperatorNode& node) = 0;
    virtual void visit(UnaryExpressionNode& node) = 0;
    virtual void visit(IntLiteralNode& node) = 0;
    virtual void visit(StringLiteralNode& node) = 0;
    virtual void visit(BinaryLiteralNode& node) = 0;
    virtual void visit(VariableDeclarationNode& node) = 0;
    virtual void visit(VariableAssignmentNode& node) = 0;
    virtual void visit(IfStatementNode& node) = 0;
    virtual void visit(WhileStatementNode& node) = 0;
    virtual void visit(BlockNode& node) = 0;
    virtual void visit(VariableExpressionNode& node) = 0;
    virtual void visit(FunctionDeclarationNode& node) = 0;
    virtual void visit(ParameterNode& node) = 0;
    virtual void visit(ReturnStatementNode& node) = 0;
    virtual void visit(FunctionCallNode& node) = 0;
    // ... other visit methods for other node types
};


class PrintVisitor : public Visitor {
public:

    void visit(ProgramNode& node) override; 
    void visit(BinaryExpressionNode& node) override; 
    void visit(UnaryExpressionNode& node) override; 
    void visit(LogicalOperatorNode& node) override; 

    void visit(IntLiteralNode& node) override; 
    void visit(StringLiteralNode& node) override; 
    void visit(BinaryLiteralNode& node) override; 
    void visit(VariableDeclarationNode& node) override; 
    void visit(VariableAssignmentNode& node) override; 

    void visit(IfStatementNode& node) override;
    void visit(WhileStatementNode& node) override;
    void visit(BlockNode& node) override;
    void visit(VariableExpressionNode& node) override;
    void visit(FunctionDeclarationNode& node) override;
    void visit(ParameterNode& node) override;
    void visit(ReturnStatementNode& node) override;
    void visit(FunctionCallNode& node) override;


    // ... implementations for other node types
};
