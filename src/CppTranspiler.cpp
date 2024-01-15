#include "include/CppTransiler.h"






//Helper Functions
std::string extractInnerTypeFromArrayTypex(const std::string& arrayType) {
    size_t start = arrayType.find('<');
    size_t end = start;

    if (start == std::string::npos) {
        throw std::runtime_error("Invalid array type format: " + arrayType);
    }

    int bracketCount = 1;
    // Iterate through the string to find the matching closing bracket
    while (bracketCount > 0 && ++end < arrayType.size()) {
        if (arrayType[end] == '<') {
            bracketCount++;
        } else if (arrayType[end] == '>') {
            bracketCount--;
        }
    }

    if (bracketCount != 0 || end == start + 1) {
        throw std::runtime_error("Invalid array type format: " + arrayType);
    }

    return arrayType.substr(start + 1, end - start - 1);
}

bool startsWithx(const std::string& fullString, const std::string& starting) {
    // Check if the last occurrence is at the start of the string
    return fullString.rfind(starting, 0) == 0;
}

bool isArrayType(std::string& type){
  return startsWithx(type, "array<");
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
      out << node.value;
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
  std::cout << "struct " << node.structName << "{\n";
  for(auto prop: node.properties){
        prop->accept(*this);
        std::cout << "\n";
  }
  std::cout << "}\n";

};


void CppTranspilerVisitor::visit(StructInitalizerListNode& node){
  std::cout << "StructInit{ ";
  for(auto prop: node.properties){
        prop->accept(*this);
  }
  std::cout << "}\n";

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
    if(isArrayType(node.variableType)){
      std::string extractedType = extractInnerTypeFromArrayTypex(node.variableType);
      out << "vector<" << extractedType << "> ";
    } else {
      out << node.variableType;
    }
    out << " " << node.variableName << " = "; //Ex: int a = 
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

  if(node.functionName == "print"){
    out << "cout << ";
    node.arguments[0]->accept(*this);
    out << "<< std::endl;\n";
    
  }
};


void CppTranspilerVisitor::visit(VariableExpressionNode& node){
  std::ostream& out = getBufferType();
  out << node.variableName << " ";
};


void CppTranspilerVisitor::visit(FunctionDeclarationNode& node){
  insideFunction = true;
  std::ostream& out = getBufferType();

  out << node.returnType << " " << node.functionName << "( ";

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
  out << node.type << " " << node.name;
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



