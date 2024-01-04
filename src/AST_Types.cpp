#include "include/AST_Types.h"
#include "include/Visitors.h"

void BinaryExpressionNode::accept(Visitor& v){
  v.visit(*this);
}


void LogicalOperatorNode::accept(Visitor& v){
  v.visit(*this);
  
}

void UnaryExpressionNode::accept(Visitor& v){
  v.visit(*this);
}

void IntLiteralNode::accept(Visitor& v){
  v.visit(*this);
}

void StringLiteralNode::accept(Visitor& v){
  v.visit(*this);
}

void BinaryLiteralNode::accept(Visitor& v){
  v.visit(*this);
}


void StructDeclarationNode::accept(Visitor& v){
  v.visit(*this);
}

void StructInitalizerListNode::accept(Visitor& v){
  v.visit(*this);
}

void StructPropertyAccessNode::accept(Visitor& v){
  v.visit(*this);
}

void StructPropertyAssignmentNode::accept(Visitor& v){
  v.visit(*this);
}

void ProgramNode::accept(Visitor& v){
  v.visit(*this);
}


void VariableDeclarationNode::accept(Visitor& v){
  v.visit(*this);
}


void VariableAssignmentNode::accept(Visitor& v){
  v.visit(*this);
}

void IfStatementNode::accept(Visitor& v){
  v.visit(*this);
}

void WhileStatementNode::accept(Visitor& v){
  v.visit(*this);
}


void BlockNode::accept(Visitor& v){
  v.visit(*this);
}


void VariableExpressionNode::accept(Visitor& v){
  v.visit(*this);
}


void FunctionDeclarationNode::accept(Visitor& v){
  v.visit(*this);
}

void ParameterNode::accept(Visitor& v){
  v.visit(*this);
}

void ReturnStatementNode::accept(Visitor& v){
  v.visit(*this);
}


void FunctionCallNode::accept(Visitor& v){
  v.visit(*this);
}


