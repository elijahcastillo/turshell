
#include "include/Interpretor.h"
#include "include/Enviorment.h"
#include "include/Runtime.h"



    void Interpreter::visit(ProgramNode& node) {
        for (auto& stmt : node.statements) {
            stmt->accept(*this);
        }
    }

    void Interpreter::visit(BinaryExpressionNode& node) {
        node.left->accept(*this);
        node.right->accept(*this);

        RuntimeVal right = evaluationStack.top(); evaluationStack.pop();
        RuntimeVal left = evaluationStack.top(); evaluationStack.pop();


        if(node.op == "+"){
          evaluationStack.push(RuntimeVal(left.getInt() + right.getInt()));
        } else if (node.op == "-"){

          evaluationStack.push(RuntimeVal(left.getInt() - right.getInt()));
        } else if (node.op == "*"){

          evaluationStack.push(RuntimeVal(left.getInt() * right.getInt()));
        } else if (node.op == "/"){

          evaluationStack.push(RuntimeVal(left.getInt() / right.getInt()));
        }


        if(node.op == "=="){
          if(left.getInt() == right.getInt()){
            return evaluationStack.push(RuntimeVal(true));
          } else {
            return evaluationStack.push(RuntimeVal(false));
          }
        }

        if(node.op == ">"){
          if(left.getInt() > right.getInt()){
            return evaluationStack.push(RuntimeVal(true));
          } else {
            return evaluationStack.push(RuntimeVal(false));
          }
        }


        if(node.op == "<"){
          /* std::cout << "<<< " << right.getInt() << " " <<  left.getInt() << "\n"; */
          if(left.getInt() < right.getInt()){
            return evaluationStack.push(RuntimeVal(true));
          } else {
            return evaluationStack.push(RuntimeVal(false));
          }
        }


    }

    void Interpreter::visit(IntLiteralNode& node) {
      evaluationStack.push(RuntimeVal(node.value));

    }

    void Interpreter::visit(VariableDeclarationNode& node) {
      RuntimeVal value = evaluateExpression(node.initializer);
      currentScope()->setVariable(node.variableName, value, VariableSettings::Declaration);
    }

    // VariableAssignmentNode
    void Interpreter::visit(VariableAssignmentNode& node) {
      RuntimeVal value = evaluateExpression(node.value);

      currentScope()->setVariable(node.variableName, value, VariableSettings::Assignment);
      
    }

    void Interpreter::visit(WhileStatementNode& node) {
        // Implement while loop logic

        RuntimeVal condition = evaluateExpression(node.condition);
        std::cout << "While Cond: " << condition.getBool() << "\n";

        while(condition.getBool()  == true){
          
          //Run code inside of {}
          evaluateExpression(node.body);

          //Recheck condition
          condition = evaluateExpression(node.condition);
          
        }
    }

    void Interpreter::visit(IfStatementNode& node) {
        // Implement if statement logic
        RuntimeVal condition = evaluateExpression(node.condition);

        //Run code inside {}
        if(condition.getBool() == true){
          evaluateExpression(node.thenBranch);
        }

    }

    void Interpreter::visit(FunctionDeclarationNode& node) {
        // Implement function declaration logic
    }

    void Interpreter::visit(ParameterNode& node){
        // Dummy implementation or throw an exception if it should not be called
        throw std::runtime_error("visit(ParameterNode&) is not implemented");
    }

    void Interpreter::visit(FunctionCallNode& node){

      if(node.functionName == "print"){
        
        //Eval first argument
        RuntimeVal value = evaluateExpression(node.arguments[0]);
        std::cout << value.getInt() << "\n";
      }
    }

    void Interpreter::visit(BlockNode& node) {

      enterNewScope();

      for (auto& stmt : node.statements) {
          stmt->accept(*this);
      }


      exitCurrentScope();

    }

    void Interpreter::visit(VariableExpressionNode& node) {
      RuntimeVal value = currentScope()->getVariable(node.variableName);
      evaluationStack.push(value);
    }

    void Interpreter::visit(ReturnStatementNode& node) {
        // Implement return statement logic
    }


    std::shared_ptr<Environment> Interpreter::currentScope() {
        return envStack.top();
    }

    void Interpreter::enterNewScope() {
      envStack.push(std::make_shared<Environment>(currentScope()));
    }

    void Interpreter::exitCurrentScope() {
      if (!envStack.empty()) {
         envStack.pop();
      } else {
          throw std::runtime_error("Scope stack underflow");
      }
    }

  RuntimeVal Interpreter::evaluateExpression(ASTNode* node) {
      node->accept(*this);

      RuntimeVal result;

      if(!evaluationStack.empty()){

        result = evaluationStack.top();
      }



      if(!evaluationStack.empty()){

        evaluationStack.pop();
      }

      return result;
  }



    // Error handling methods
    void Interpreter::runtimeError(const std::string& message) {
        // Implement error handling
        std::cerr << "Runtime Error: " << message << std::endl;
        // You can throw an exception or handle it in another way
    }


void Interpreter::printStack() {
    std::stack<RuntimeVal> tempStack = evaluationStack; // Make a copy to iterate

    std::cout << "\nStack contents:" << std::endl;
    while (!tempStack.empty()) {
        RuntimeVal val = tempStack.top();
        tempStack.pop();
        std::cout << val.getInt() << std::endl; // Assuming RuntimeVal has toString
    }
}

void Interpreter::printEnv(){
  for (const auto& pair : currentScope()->variables) {
        std::cout << "Var: " << pair.first << ", Value: " << pair.second.getInt() << std::endl;
    }
}


