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

void PrintVisitor::visit(FloatLiteralNode& node) {
        std::cout << node.value;
}

void PrintVisitor::visit(StringLiteralNode& node) {
        std::cout << node.value;
}

void PrintVisitor::visit(BinaryLiteralNode& node) {
        std::cout << (node.value ? "true" : "false");
}

void PrintVisitor::visit(ArrayLiteralNode& node) {
  std::cout << "[";
  for(auto val: node.values){
        val->accept(*this);
        std::cout << ", ";
  }
  std::cout << "]";
}





void PrintVisitor::visit(ArrayAccessNode& node) {
  std::cout << "[";
  node.index->accept(*this);
  std::cout << "]";
}

void PrintVisitor::visit(PropertyAccessNode& node) {
  std::cout << "." << node.propertyName;
}

void PrintVisitor::visit(ChainedAccessNode& node) {
  std::cout << "ChainedAccess: ";
  for(auto val: node.accesses){
        val->accept(*this);
  }
}

void PrintVisitor::visit(ChainedAssignmentNode& node) {
  std::cout << "ChainedAssignment: ";
  node.accesses->accept(*this);
  std::cout << " = ";
  node.value->accept(*this);
}


//=======
void PrintVisitor::visit(StructMethodDeclarationNode& node){
  std::cout << "impl " << node.structName << "->";
  node.methodDeclaration->accept(*this);

};

void PrintVisitor::visit(StructMethodCallNode& node){
  std::cout << "Struct Method " << node.methodName << ".";
  node.functionCall->accept(*this);

};

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


void PrintVisitor::visit(StructPropertyAccessNode& node){
  std::cout << node.baseName;
  std::cout << ".";
  for(int i = 0; i < node.propertyNames.size(); i++){
    std::cout << node.propertyNames[i];
    if(!(i == node.propertyNames.size() - 1)){
      std::cout << ".";
    }
  }
};

void PrintVisitor::visit(StructPropertyAssignmentNode& node){
  std::cout << node.baseName;
  std::cout << ".";
  for(int i = 0; i < node.propertyNames.size(); i++){
    std::cout << node.propertyNames[i];
    if(!(i == node.propertyNames.size() - 1)){
      std::cout << ".";
    }
  }
  std::cout << " = ";
  node.value->accept(*this);
  std::cout << "\n";
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

void PrintVisitor::visit(ForStatementNode& node){
    std::cout << "ForStmt: if( ";
    if(node.initializer){

      node.initializer->accept(*this); 
    }
    std::cout << "; ";

    node.condition->accept(*this); 
    std::cout << "; ";


    if(node.update){

      node.update->accept(*this); 
    }
    std::cout << "; ";


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
  if(node.expression){
    node.expression->accept(*this);
  }
  std::cout <<";\n";
};

void PrintVisitor::visit(BreakStatementNode& node){
  std::cout << "break";
};



