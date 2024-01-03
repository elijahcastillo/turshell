#include "include/Visitors.h"
#include "include/AST_Types.h"



void PrintVisitor::visit(ProgramNode& node) {
      for(auto stmt: node.statements){

        stmt->accept(*this);
      }
}

void PrintVisitor::visit(BinaryExpressionNode& node){
        std::cout << "(";
        node.left->accept(*this);
        std::cout << " " << node.op << " ";
        node.right->accept(*this);
        std::cout << ")";
}


void PrintVisitor::visit(LogicalOperatorNode& node){
        std::cout << "(";
        node.left->accept(*this);
        std::cout << " " << node.op << " ";
        node.right->accept(*this);
        std::cout << ")";
}


void PrintVisitor::visit(UnaryExpressionNode& node){
        std::cout << "(";
        std::cout << node.op;
        node.right->accept(*this);
        std::cout << ")";
}

void PrintVisitor::visit(IntLiteralNode& node) {
        std::cout << node.value;
}

void PrintVisitor::visit(StringLiteralNode& node) {
        std::cout << node.value;
}

void PrintVisitor::visit(BinaryLiteralNode& node) {
        std::cout << (node.value ? "true" : "false");
}

void PrintVisitor::visit(StructDeclarationNode& node){
  std::cout << "struct " << node.structName << "{\n";
  for(auto prop: node.properties){
        prop->accept(*this);
        std::cout << "\n";
  }
  std::cout << "}\n";

};


void PrintVisitor::visit(StructInitalizerListNode& node){
  std::cout << "StructInit{ ";
  for(auto prop: node.properties){
        prop->accept(*this);
  }
  std::cout << "}\n";

};

void PrintVisitor::visit(VariableDeclarationNode& node){
  std::cout << "Variable Declaration: " << node.variableType << " " << node.variableName << " = " ;
    node.initializer->accept(*this);
    std::cout << "\n";
}; 


void PrintVisitor::visit(VariableAssignmentNode& node){
  std::cout << "Variable Assigment: " << node.variableName << " = " ;
    node.value->accept(*this);
    std::cout << "\n";
}; 


void PrintVisitor::visit(IfStatementNode& node){

    std::cout << "IfStmt: if( ";
    node.condition->accept(*this); 
    std::cout << " ){\n";

    node.thenBranch->accept(*this); 

    std::cout << " }\n";
};




void PrintVisitor::visit(WhileStatementNode& node){
    std::cout << "WhileStmt: while( ";
    node.condition->accept(*this); 
    std::cout << " ){\n";

    node.body->accept(*this); 

    std::cout << " }\n";
};

void PrintVisitor::visit(BlockNode& node){
    for(auto stmt: node.statements){
        stmt->accept(*this);
    }
};


void PrintVisitor::visit(FunctionCallNode& node){
  std::cout << "Func Call: " <<  node.functionName << "\n";
};


void PrintVisitor::visit(VariableExpressionNode& node){
  std::cout << node.variableName;
};


void PrintVisitor::visit(FunctionDeclarationNode& node){
  std::cout << "Func Decl: func " << node.functionName << "("; 
  
  for(auto param: node.parameters){
    param->accept(*this);
  }

  std::cout << ") " << node.returnType << "{\n";
  node.body->accept(*this);
  std::cout << "}\n";


};

void PrintVisitor::visit(ParameterNode& node){
  std::cout << node.type << " " <<  node.name << " ";
};

void PrintVisitor::visit(ReturnStatementNode& node){
  std::cout << "return ";
  node.expression->accept(*this);
  std::cout <<";\n";
};



