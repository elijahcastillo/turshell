#include "include/AST_Types.h"
#include "include/CppTranspiler.h"
#include "include/TurshellHelp.h"
#include <sstream>
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
        isStandaloneStatement = false; // Set to false for expression context

        std::stringstream& out = getBufferType();
        out << "(";
        node.left->accept(*this);
        out << " " << node.op << " ";
        node.right->accept(*this);
        out << ")";

        isStandaloneStatement = true; // Reset after expression is handled
}


void CppTranspilerVisitor::visit(LogicalOperatorNode& node){

        isStandaloneStatement = false; // Set to false for expression context

        std::stringstream& out = getBufferType();
        out << "(";
        node.left->accept(*this);
        out << " " << node.op << " ";
        node.right->accept(*this);
        out << ")";


        isStandaloneStatement = true; // Reset after expression is handled
}


void CppTranspilerVisitor::visit(UnaryExpressionNode& node){


        isStandaloneStatement = false; // Set to false for expression context

        std::stringstream& out = getBufferType();
        out << "(";
        out << node.op;
        node.right->accept(*this);
        out << ")";


        isStandaloneStatement = true; // Reset after expression is handled
}

void CppTranspilerVisitor::visit(IntLiteralNode& node) {

      std::stringstream& out = getBufferType();
      out << node.value;
}

void CppTranspilerVisitor::visit(FloatLiteralNode& node) {
      std::stringstream& out = getBufferType();
      out << node.value;
}

void CppTranspilerVisitor::visit(StringLiteralNode& node) {
      std::stringstream& out = getBufferType();
      out << "\""  << node.value << "\"";
}

void CppTranspilerVisitor::visit(BinaryLiteralNode& node) {
      std::stringstream& out = getBufferType();
      out << (node.value ? "true" : "false");
}

void CppTranspilerVisitor::visit(ArrayLiteralNode& node) {

  std::stringstream& out = getBufferType();
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
  std::stringstream& out = getBufferType();
  out << "[";
  node.index->accept(*this);
  out << "]";
}

void CppTranspilerVisitor::visit(PropertyAccessNode& node) {


  std::stringstream& out = getBufferType();
  out << "." << node.propertyName;
}

void CppTranspilerVisitor::visit(ChainedAccessNode& node) {

  std::stringstream& out = getBufferType();


  int i = 0;

  if(isInsideStruct()){

      VariableExpressionNode* first = dynamic_cast<VariableExpressionNode*>(node.accesses[0]);
      PropertyAccessNode* second = dynamic_cast<PropertyAccessNode*>(node.accesses[1]);
      if(first->variableName == "self"){
        out << "this->";
        
        //Ex: self.x  ->  x
        if(second){
         out << second->propertyName;
         i += 2;
        }

      }
  }



  for(; i < node.accesses.size(); i++){
        node.accesses[i]->accept(*this);
  }
  


}

void CppTranspilerVisitor::visit(ChainedAssignmentNode& node) {

  isStandaloneStatement = false; // Set to false for expression context

  std::stringstream& out = getBufferType();
  node.accesses->accept(*this);
  out << " = ";
  node.value->accept(*this);
  out << ";\n";

  isStandaloneStatement = true; // Set to false for expression context

}


void CppTranspilerVisitor::visit(StructMethodDeclarationNode& node){
    for (const auto& pair : structDeclarations) {
      std::cout << "Found struct (method)!!: " << pair.first << "\n";
    }

            


    std::cout << "Visiting Struct Method Declaration for struct: " << node.structName << "\n";
    if(!structDeclarationExists(node.structName)){
        throw std::runtime_error("C++ Transpiler: struct declaration does not exist for " + node.structName);
    }

    m_currentStructName = node.structName;
    std::cout << "Current Struct Name set to: " << m_currentStructName << "\n";




  insideStructDecl = true;
  std::stringstream& out = getBufferType();

  //Function Declaration node
  node.methodDeclaration->accept(*this);

  insideStructDecl = false;
  m_currentStructName.clear(); // Clear the current struct name

};

void CppTranspilerVisitor::visit(StructMethodCallNode& node){

  std::stringstream& out = getBufferType();
  out << ".";
  node.functionCall->accept(*this);
};



void CppTranspilerVisitor::visit(StructDeclarationNode& node) {
    std::cout << "Visiting Struct Declaration: " << node.structName << "\n";
    if (structDeclarationExists(node.structName)) {
        throw std::runtime_error("Struct already declared: " + node.structName);
    }

    structDeclarations[node.structName] = std::stringstream();
    m_currentStructName = node.structName;
    insideStructDecl = true;

    std::stringstream& out = getBufferType();
    for (auto prop : node.properties) {
        prop->accept(*this);
        out << ";\n";
    }

    insideStructDecl = false;
    m_currentStructName.clear();
    std::cout << "Completed Struct Declaration: " << node.structName << "\n";
}




void CppTranspilerVisitor::visit(StructInitalizerListNode& node){


  isStandaloneStatement = false; // Set to false for expression context
  
  std::stringstream& out = getBufferType();
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


  isStandaloneStatement = true; // Set to false for expression context

};


//These 2 are depreciated I think?
void CppTranspilerVisitor::visit(StructPropertyAccessNode& node){
};

void CppTranspilerVisitor::visit(StructPropertyAssignmentNode& node){
};


//Ex: int a = 3;
void CppTranspilerVisitor::visit(VariableDeclarationNode& node){

    isStandaloneStatement = false; // Set to false for expression context
                                 
    std::stringstream& out = getBufferType();
    out << convertTurshellType(node.variableType) <<  " " << node.variableName << " = "; //Ex: int a = 
    node.initializer->accept(*this);
    out << ";\n";


    isStandaloneStatement = true; // Set to false for expression context


}; 


//Ex a = 3;
void CppTranspilerVisitor::visit(VariableAssignmentNode& node){

    isStandaloneStatement = false; // Set to false for expression context
    std::stringstream& out = getBufferType();
    out << node.variableName << " = "; //Ex: a = 
    node.value->accept(*this);
    out << ";\n";

    isStandaloneStatement = true; // Set to false for expression context
}; 


void CppTranspilerVisitor::visit(IfStatementNode& node){

    std::stringstream& out = getBufferType();
    out << "if( ";

    isStandaloneStatement = false; // Set to false for expression context
    node.condition->accept(*this); 
    isStandaloneStatement = true; // Set to false for expression context
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

    std::stringstream& out = getBufferType();
    out << "while( ";

    isStandaloneStatement = false; // Set to false for expression context
    node.condition->accept(*this); 
    isStandaloneStatement = true; // Set to false for expression context
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
    auto it = nativeFunctionMap.find(node.functionName);
    if (it != nativeFunctionMap.end()) {
        // Native function found, call the mapped function
        it->second(node);
        return;
    }

  //TODO: Native functions support
  /* if(node.functionName == "print"){ */
  /*  */
  /*   isStandaloneStatement = false; */
  /*   out << "cout << "; */
  /* for(int i = 0; i < node.arguments.size(); i++){ */
  /*  */
  /*   node.arguments[i]->accept(*this); */
  /*   if(i != node.arguments.size() - 1){ */
  /*     out << " << \" \" << "; */
  /*   } */
  /* } */
  /*  */
  /* isStandaloneStatement = true; */
  /*   out << "<< std::endl;\n"; */
  /*   return; */
  /* } else if (node.functionName == "len"){ */
  /*    */
  /*   node.arguments[0]->accept(*this); */
  /*   out << ".size()"; */
  /*   return; */
  /* } */



  std::stringstream& out = getBufferType();

  //Normal User defined function call
  out << node.functionName << "( ";
  for(int i = 0; i < node.arguments.size(); i++){
    node.arguments[i]->accept(*this);

    if(i != node.arguments.size() - 1){
      out << ", ";
    }
  }
  out << " )";

    // Append a semicolon if it's a standalone statement
    if (isStandaloneStatement) {
        out << ";";
    }

    // Reset the flag for the next node
    isStandaloneStatement = true;


};


void CppTranspilerVisitor::visit(VariableExpressionNode& node){
  std::stringstream& out = getBufferType();

  /* if(isInsideStruct() && node.variableName == "self"){ */
  /*   return; //We dont need self in struct impl */
  /* } */

  out << node.variableName;
};


void CppTranspilerVisitor::visit(FunctionDeclarationNode& node){
  //If in a struct append to struct not function buffer
    std::cout << "Visiting Function Declaration: " << node.functionName << "\n";
    if (!isInsideStruct()) {
        insideFunction = true;
    }

  std::stringstream& out = getBufferType();

  out << convertTurshellType(node.returnType) << " " << node.functionName << "( ";

  int i = 0;
  if(isInsideStruct()){
   i = 1; //Skips the self
  }

  for(; i < node.parameters.size(); i++){
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

  std::stringstream& out = getBufferType();
  out << convertTurshellType(node.type) << " " << node.name;
};

void CppTranspilerVisitor::visit(ReturnStatementNode& node){

  std::stringstream& out = getBufferType();
  out << "return ";
  if(node.expression){
    node.expression->accept(*this);
  }
  out <<";\n";
};

void CppTranspilerVisitor::visit(BreakStatementNode& node){

  std::stringstream& out = getBufferType();
  out << "break;";
};






// ==============================
//        Native Functions
// ==============================
void CppTranspilerVisitor::setupNativeFunctions() {
    nativeFunctionMap["print"] = [this](FunctionCallNode& node) {
        std::stringstream& out = getBufferType();
        out << "cout << ";
        for (int i = 0; i < node.arguments.size(); ++i) {

            isStandaloneStatement = false;
            node.arguments[i]->accept(*this);
            isStandaloneStatement = true;

            if (i != node.arguments.size() - 1) {
                out << " << \" \" << "; //Add the space
            }
        }
        out << "<< std::endl;";
    };


    nativeFunctionMap["len"] = [this](FunctionCallNode& node) {
      //Should only work with arrays and strings
        std::stringstream& out = getBufferType();
       node.arguments[0]->accept(*this);
       out << ".size()";
    };

}


