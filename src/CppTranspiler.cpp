#include "include/CppTransiler.h"
#include "include/TurshellHelp.h"
#include <stdexcept>






bool isArrayType(std::string& type){
  return startsWith(type, "array<");
}

std::string  CppTranspilerVisitor::convertTurshellType(std::string type){
  if(type == "int" || type == "float" || type == "bool" || type == "string"){
    return type;
  } else if (isArrayType(type)){

    //This code is to handle arrays and nested arrays
    std::string final_arr_type = "vector<";
    std::string extracted = extractInnerTypeFromArrayType(type);

    int num_end = 1;

    while(isArrayType(extracted)){
      final_arr_type += "vector<";
      extracted = extractInnerTypeFromArrayType(extracted);
      num_end++;
    }


    final_arr_type += extracted;

    for(int i = 0; i < num_end; i++){
      final_arr_type += ">";
    }

    return final_arr_type;


  } else {
    //Expected to be a struct type
    return type;
  }

}







void CppTranspilerVisitor::visit(ProgramNode& node) {
      for(auto stmt: node.statements){
        stmt->accept(*this);
      }
}

void CppTranspilerVisitor::visit(BinaryExpressionNode& node){
        std::ostream& out = getBufferType();
        out << "(";
        node.left->accept(*this);
        out << " " << node.op << " ";
        node.right->accept(*this);
        out << ")";
}


void CppTranspilerVisitor::visit(LogicalOperatorNode& node){
        std::ostream& out = getBufferType();
        out << "(";
        node.left->accept(*this);
        out << " " << node.op << " ";
        node.right->accept(*this);
        out << ")";
}


void CppTranspilerVisitor::visit(UnaryExpressionNode& node){

        std::ostream& out = getBufferType();
        out << "(";
        out << node.op;
        node.right->accept(*this);
        out << ")";
}

void CppTranspilerVisitor::visit(IntLiteralNode& node) {

      std::ostream& out = getBufferType();
      out << node.value;
}

void CppTranspilerVisitor::visit(FloatLiteralNode& node) {
      std::ostream& out = getBufferType();
      out << node.value;
}

void CppTranspilerVisitor::visit(StringLiteralNode& node) {
      std::ostream& out = getBufferType();
      out << "\""  << node.value << "\"";
}

void CppTranspilerVisitor::visit(BinaryLiteralNode& node) {
      std::ostream& out = getBufferType();
      out << (node.value ? "true" : "false");
}

void CppTranspilerVisitor::visit(ArrayLiteralNode& node) {

  std::ostream& out = getBufferType();
  out << "{";
  for(int i = 0; i < node.values.size(); i++){
    node.values[i]->accept(*this);

    if(i != node.values.size() - 1){
      out << ", ";
    }
  }
  out << "}";
}





void CppTranspilerVisitor::visit(ArrayAccessNode& node) {
  std::cout << "[";
  node.index->accept(*this);
  std::cout << "]";
}

void CppTranspilerVisitor::visit(PropertyAccessNode& node) {
  std::cout << "." << node.propertyName;
}

void CppTranspilerVisitor::visit(ChainedAccessNode& node) {
  std::cout << "ChainedAccess: ";
  for(auto val: node.accesses){
        val->accept(*this);
  }
}

void CppTranspilerVisitor::visit(ChainedAssignmentNode& node) {
  std::cout << "ChainedAssignment: ";
  node.accesses->accept(*this);
  std::cout << " = ";
  node.value->accept(*this);
}


void CppTranspilerVisitor::visit(StructMethodDeclarationNode& node){
  std::cout << "impl " << node.structName << "->";
  node.methodDeclaration->accept(*this);

};

void CppTranspilerVisitor::visit(StructMethodCallNode& node){
  std::cout << "Struct Method " << node.methodName << ".";
  node.functionCall->accept(*this);

};

void CppTranspilerVisitor::visit(StructDeclarationNode& node){
  insideStructDecl = true;
  std::ostream& out = getBufferType();

  out << "struct " << node.structName << "{\n";
  for(auto prop: node.properties){
        prop->accept(*this);
        out << ";\n";
  }

  out << "};\n";

  insideStructDecl = false;

};


void CppTranspilerVisitor::visit(StructInitalizerListNode& node){
  
  std::ostream& out = getBufferType();
  out << "{ ";
  for(int i = 0; i < node.properties.size(); i++){
    out << "."; 
    VariableAssignmentNode* varAssign = dynamic_cast<VariableAssignmentNode*>(node.properties[i]);
    if (!varAssign) {
        throw std::runtime_error("Invalid property in struct initializer list");
    }
    out << varAssign->variableName << " = ";
    varAssign->value->accept(*this);

    if(!(i == node.properties.size() - 1)){
      out << ", ";
    }
  }

  out << "}";

};


void CppTranspilerVisitor::visit(StructPropertyAccessNode& node){
  std::cout << node.baseName;
  std::cout << ".";
  for(int i = 0; i < node.propertyNames.size(); i++){
    std::cout << node.propertyNames[i];
    if(!(i == node.propertyNames.size() - 1)){
      std::cout << ".";
    }
  }
};

void CppTranspilerVisitor::visit(StructPropertyAssignmentNode& node){
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


//Ex: int a = 3;
void CppTranspilerVisitor::visit(VariableDeclarationNode& node){
    std::ostream& out = getBufferType();
    
    out << convertTurshellType(node.variableType) <<  " " << node.variableName << " = "; //Ex: int a = 
    node.initializer->accept(*this);
    out << ";\n";


}; 


//Ex a = 3;
void CppTranspilerVisitor::visit(VariableAssignmentNode& node){
    std::ostream& out = getBufferType();
    out << node.variableName << " = "; //Ex: a = 
    node.value->accept(*this);
    out << ";\n";
}; 


void CppTranspilerVisitor::visit(IfStatementNode& node){

    std::ostream& out = getBufferType();
    out << "if( ";
    node.condition->accept(*this); 
    out << " ){\n";
    node.thenBranch->accept(*this); 
    out << " }";

    if(node.elseBranch){
      out << " else {\n";
      node.elseBranch->accept(*this); 
      out << "}";
    }

    out << "\n";

};




void CppTranspilerVisitor::visit(WhileStatementNode& node){

    std::ostream& out = getBufferType();
    out << "while( ";
    node.condition->accept(*this); 
    out << " ){\n";
    node.body->accept(*this); 
    out << "}\n";

};

void CppTranspilerVisitor::visit(ForStatementNode& node){
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


void CppTranspilerVisitor::visit(BlockNode& node){
    for(auto stmt: node.statements){
        stmt->accept(*this);
    }
};


void CppTranspilerVisitor::visit(FunctionCallNode& node){
  std::ostream& out = getBufferType();

  //TODO: Native functions support
  if(node.functionName == "print"){
    out << "cout << ";
  for(int i = 0; i < node.arguments.size(); i++){
    node.arguments[i]->accept(*this);
    if(i != node.arguments.size() - 1){
      out << " << \" \" << ";
    }


  }
    out << "<< std::endl;\n";
    return;
  }

  out << node.functionName << "( ";
  for(int i = 0; i < node.arguments.size(); i++){
    node.arguments[i]->accept(*this);

    if(i != node.arguments.size() - 1){
      out << ", ";
    }
  }
  out << " )";


};


void CppTranspilerVisitor::visit(VariableExpressionNode& node){
  std::ostream& out = getBufferType();
  out << node.variableName << " ";
};


void CppTranspilerVisitor::visit(FunctionDeclarationNode& node){
  insideFunction = true;
  std::ostream& out = getBufferType();

  out << convertTurshellType(node.returnType) << " " << node.functionName << "( ";

  for(int i = 0; i < node.parameters.size(); i++){
    node.parameters[i]->accept(*this);

    if(i != node.parameters.size() - 1){
      out << ", ";
    }
  }
  out << " ){\n";
  node.body->accept(*this);
  out << "}\n";

  insideFunction = false;


};

void CppTranspilerVisitor::visit(ParameterNode& node){

  std::ostream& out = getBufferType();
  out << convertTurshellType(node.type) << " " << node.name;
};

void CppTranspilerVisitor::visit(ReturnStatementNode& node){

  std::ostream& out = getBufferType();
  out << "return ";
  if(node.expression){
    node.expression->accept(*this);
  }
  out <<";\n";
};

void CppTranspilerVisitor::visit(BreakStatementNode& node){

  std::ostream& out = getBufferType();
  out << "break;";
};



