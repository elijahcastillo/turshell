
#include "include/Interpretor.h"
#include "include/Enviorment.h"
#include "include/Runtime.h"
#include <memory>
#include <stdexcept>
#include <string>



//HelperFunctions and Classes
class TurshellReturn : public std::exception {

    public:
    std::shared_ptr<RuntimeVal> returnVal;
    const char* msg;

    TurshellReturn(std::shared_ptr<RuntimeVal> returnVal, const char* msg) : returnVal(returnVal), msg(msg) {};

    const char * what () {
        return msg;
    }
};





//======== Native Functions ==========
void Interpreter::registerNativeFunction(const std::string& name, std::function<std::shared_ptr<RuntimeVal>(Interpreter&, std::vector<std::shared_ptr<RuntimeVal>>&)> func) {
    nativeFunctions[name] = func;
}

std::shared_ptr<RuntimeVal> Interpreter::callNativeFunction(const std::string& name, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    auto it = nativeFunctions.find(name);
    if (it != nativeFunctions.end()) {
        return it->second(*this, args);
    }
    runtimeError("Native function not found: " + name);
}
//======= End Native FUnctions ========



    void Interpreter::visit(ProgramNode& node) {
        for (auto& stmt : node.statements) {
            stmt->accept(*this);
            while(!evaluationStack.empty()){
              evaluationStack.pop();
            }
        }
    }

    void Interpreter::visit(BinaryExpressionNode& node) {
        node.left->accept(*this);
        node.right->accept(*this);

        std::shared_ptr<RuntimeVal> right = evaluationStack.top(); evaluationStack.pop();
        std::shared_ptr<RuntimeVal> left = evaluationStack.top(); evaluationStack.pop();


        if(left->getType() == "int" && right->getType() == "int"){
          auto leftInt = static_cast<IntValue*>(left.get());
          auto rightInt = static_cast<IntValue*>(right.get());

          if(node.op == "+"){

            /* std::cout << "Adding " << leftInt->getValue() << " to " << rightInt->getValue() << std::endl; */
            return evaluationStack.push(std::make_shared<IntValue>(leftInt->getValue() + rightInt->getValue()));
          }
          if(node.op == "-"){
            return evaluationStack.push(std::make_shared<IntValue>(leftInt->getValue() - rightInt->getValue()));
          }
          if(node.op == "*"){
            return evaluationStack.push(std::make_shared<IntValue>(leftInt->getValue() * rightInt->getValue()));
          }
          if(node.op == "/"){
            return evaluationStack.push(std::make_shared<IntValue>(leftInt->getValue() / rightInt->getValue()));
          }
          if(node.op == "%"){
            return evaluationStack.push(std::make_shared<IntValue>(leftInt->getValue() % rightInt->getValue()));
          }
          if(node.op == "=="){
            return evaluationStack.push(std::make_shared<BoolValue>(leftInt->getValue() == rightInt->getValue() ? true : false));
          }
          if(node.op == ">"){
            return evaluationStack.push(std::make_shared<BoolValue>(leftInt->getValue() > rightInt->getValue() ? true : false));
          }
          if(node.op == "<"){
            return evaluationStack.push(std::make_shared<BoolValue>(leftInt->getValue() < rightInt->getValue() ? true : false));
          }
          if(node.op == ">="){
            return evaluationStack.push(std::make_shared<BoolValue>(leftInt->getValue() >= rightInt->getValue() ? true : false));
          }
          if(node.op == "<="){
            /* std::cout << "Comparing " << leftInt->getValue() << " <= " << rightInt->getValue() << std::endl; */
            return evaluationStack.push(std::make_shared<BoolValue>(leftInt->getValue() <= rightInt->getValue() ? true : false));
          }

        }


        if(left->getType() == "string" && right->getType() == "string"){
          auto leftStr = static_cast<StringValue*>(left.get());
          auto rightStr = static_cast<StringValue*>(right.get());

          if(node.op == "+"){
            return evaluationStack.push(std::make_shared<StringValue>(leftStr->getValue() + rightStr->getValue()));
          }
          if(node.op == "=="){
            return evaluationStack.push(std::make_shared<BoolValue>(leftStr->getValue() == rightStr->getValue()));
          }
        }


        if(left->getType() == "bool" && right->getType() == "bool"){
          auto leftBool = static_cast<BoolValue*>(left.get());
          auto rightBool = static_cast<BoolValue*>(right.get());

          if(node.op == "=="){
            return evaluationStack.push(std::make_shared<BoolValue>(leftBool->getValue() == rightBool->getValue()));
          }
        }



      runtimeError("Unsuporrted Binary Expression");

    }


    //&& ||
    void Interpreter::visit(LogicalOperatorNode& node) {
      
      std::shared_ptr<RuntimeVal> left = evaluateExpression(node.left);
      std::shared_ptr<RuntimeVal> right = evaluateExpression(node.right);


      if(left->getType() == "bool" && right->getType() == "bool"){
          auto leftBool = static_cast<BoolValue*>(left.get());
          auto rightBool = static_cast<BoolValue*>(right.get());


        if(node.op == "&&"){
          return evaluationStack.push(std::make_shared<BoolValue>(leftBool->getValue() && rightBool->getValue()));
        }

        if(node.op == "||"){
          return evaluationStack.push(std::make_shared<BoolValue>(leftBool->getValue() || rightBool->getValue()));

        }

      }


      runtimeError("Unsuporrted Logical Expression");


    }



    void Interpreter::visit(UnaryExpressionNode& node) {
      std::shared_ptr<RuntimeVal> right = evaluateExpression(node.right);

      if(right->getType() == "int"){
        auto rightInt = static_cast<IntValue*>(right.get());

        if(node.op == "-"){
          return evaluationStack.push(std::make_shared<IntValue>(rightInt->getValue() * -1));
        }
      }

      if(right->getType() == "bool"){
        auto rightBool = static_cast<BoolValue*>(right.get());

        if(node.op == "!"){
          if(rightBool->getValue() == true){
            return evaluationStack.push(std::make_shared<BoolValue>(false));
          } else {

            return evaluationStack.push(std::make_shared<BoolValue>(true));
          }

        }
      }


      runtimeError("Unsuporrted Unary Expression");

    }

    void Interpreter::visit(IntLiteralNode& node) {
      evaluationStack.push(std::make_shared<IntValue>(node.value));

    }

    void Interpreter::visit(StringLiteralNode& node) {
      evaluationStack.push(std::make_shared<StringValue>(node.value));

    }

    void Interpreter::visit(BinaryLiteralNode& node) {
      evaluationStack.push(node.value ? std::make_shared<BoolValue>(true) : std::make_shared<BoolValue>(false));

    }

    void Interpreter::visit(VariableDeclarationNode& node) {
      std::shared_ptr<RuntimeVal> value = evaluateExpression(node.initializer);

      if(!(node.variableType == value->getType())){
          runtimeError("Type mismatch in variable declaration for " + node.variableName);
          return;
      }


        currentScope()->setVariable(node.variableName, value, VariableSettings::Declaration);
    }

    // VariableAssignmentNode
    void Interpreter::visit(VariableAssignmentNode& node) {
      std::shared_ptr<RuntimeVal> value = evaluateExpression(node.value);
      std::shared_ptr<RuntimeVal> assignTo = currentScope()->getVariable(node.variableName);

      if(value->getType() != assignTo->getType()){
        runtimeError("Cannot assign value of type '" + value->getType() + "' to variable '" + node.variableName + "' of type '" + assignTo->getType() + "'");
        return;
      }



      currentScope()->setVariable(node.variableName, value, VariableSettings::Assignment);
      
    }

    void Interpreter::visit(WhileStatementNode& node) {
        // Implement while loop logic

      std::shared_ptr<RuntimeVal> condition = evaluateExpression(node.condition);

      if(condition->getType() != "bool"){
        runtimeError("Cannot evaluate 'while' condition of on boolean type");
      }

      auto conditionValue = static_cast<BoolValue*>(condition.get());


        while(conditionValue->getValue()  == true){
          
          //Run code inside of {}
          enterNewScope();
          evaluateExpression(node.body);
          exitCurrentScope();

          //Recheck condition
          condition = evaluateExpression(node.condition);

          if(condition->getType() != "bool"){
            runtimeError("Cannot evaluate 'while' condition of on boolean type");
          }

          conditionValue = static_cast<BoolValue*>(condition.get());
          
        }
    }

    void Interpreter::visit(IfStatementNode& node) {
        // Implement if statement logic
      std::shared_ptr<RuntimeVal> condition = evaluateExpression(node.condition);

      if(condition->getType() != "bool"){
        runtimeError("Cannot evaluate 'if' condition of on boolean type");
      }


      auto conditionValue = static_cast<BoolValue*>(condition.get());

        //Run code inside {}
        if(conditionValue->getValue() == true){
          enterNewScope();
          evaluateExpression(node.thenBranch);
          exitCurrentScope();
        }

    }



    void Interpreter::visit(StructDeclarationNode& node) {
      //Do something Here not sure yet, its a user defined type
    }


    void Interpreter::visit(FunctionDeclarationNode& node) {
        if (functionTable.find(node.functionName) != functionTable.end()) {
            // Function with the same name already exists
            runtimeError("Function '" + node.functionName + "' is already declared.");
        }
        functionTable[node.functionName] = &node;
    }

    void Interpreter::visit(ParameterNode& node){
        // Dummy implementation or throw an exception if it should not be called
        throw std::runtime_error("visit(ParameterNode&) is not implemented");
    }






    void Interpreter::visit(FunctionCallNode& node){



        // Get the function declaration
        FunctionDeclarationNode* functionDecl = nullptr;
        bool isNativeFunction = false;

        // Check if the function is declared by user
        if (functionTable.find(node.functionName) != functionTable.end()) {
          functionDecl = functionTable[node.functionName];
        }

        // Check if the function is a native function
        if (nativeFunctions.find(node.functionName) != nativeFunctions.end()) {
          isNativeFunction = true;
        }

        //No function
        if(functionDecl == nullptr && isNativeFunction == false){
          runtimeError("Function '" + node.functionName + "' is not declared.");
        }

        // Check the number of arguments
        if (!isNativeFunction && node.arguments.size() != functionDecl->parameters.size()) {
            runtimeError("Function '" + node.functionName + "' called with the wrong number of arguments. Got " + std::to_string(node.arguments.size()) + " expected " + std::to_string(functionDecl->parameters.size()));
        }


        // Create a new local scope for the function call parameters, since visit block node creates its own scope




        if(isNativeFunction){
          std::vector<std::shared_ptr<RuntimeVal>> arguments;
          for(int i = 0; i < node.arguments.size(); i++){
            arguments.push_back(evaluateExpression(node.arguments[i]));
          }
          std::shared_ptr<RuntimeVal> returnValue = callNativeFunction(node.functionName, arguments);

          evaluationStack.push(returnValue);
          return;
        }


        enterNewScope();




        // Bind arguments to parameters in the new local scope
        for (size_t i = 0; i < node.arguments.size(); ++i) {
            
            ParameterNode* declParam = dynamic_cast<ParameterNode*>(functionDecl->parameters[i]);
            ParameterNode* callParam = dynamic_cast<ParameterNode*>(node.arguments[i]);

            


            std::string paramName = declParam->name;
            std::shared_ptr<RuntimeVal> argValue = evaluateExpression(node.arguments[i]);

            //Check type of parameters to fuction declaration
            if(argValue->getType() != declParam->type){
              runtimeError("Invalid function parameter type of '" + argValue->getType() + "' expected '" + declParam->type + "' in function '" + functionDecl->functionName + "'");
              return;
            }

            currentScope()->setVariable(paramName, argValue, VariableSettings::Declaration);
        }


        //Evalute body of function
        bool didReturn = false;
        std::shared_ptr<RuntimeVal> returnValue;

          try{

            evaluateExpression(functionDecl->body);

          } catch(TurshellReturn e) {
            //End Body execution early
            returnValue = e.returnVal;
            didReturn = true;

          }


        // Pop the local scope from the stack
        exitCurrentScope();


        if(didReturn){

          if(returnValue->getType() != functionDecl->returnType){
            runtimeError("Invalid return type of '" + returnValue->getType() + "' expected '" + functionDecl->returnType + "' in function '" + functionDecl->functionName + "'" );
            return;
          }


          // Push the return value onto the stack
          evaluationStack.push(returnValue);
        }



    }



    void Interpreter::visit(BlockNode& node) {

      /* enterNewScope(); */


      for (auto& stmt : node.statements) {

            stmt->accept(*this);
      }


      /* exitCurrentScope(); */

    }

    void Interpreter::visit(VariableExpressionNode& node) {
      std::shared_ptr<RuntimeVal> value = currentScope()->getVariable(node.variableName);
      /* std::cout << "Varaible Expression Node pushed: " << value->getType() << "  " << value->toString() << "\n"; */
      evaluationStack.push(value);
    }

    void Interpreter::visit(ReturnStatementNode& node) {
           // Evaluate the expression to be returned
      std::shared_ptr<RuntimeVal> returnValue;
        if (node.expression) {
            returnValue = evaluateExpression(node.expression);
        }



        // Exit the current function early, use tryCatch to stop execution in FuncCall
        // If error not caught my FuncCall Visitor then its not in a function
        throw TurshellReturn(returnValue, "Return Error - Can only use return inside of function");
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

std::shared_ptr<RuntimeVal> Interpreter::evaluateExpression(ASTNode* node) {
      node->accept(*this);

      std::shared_ptr<RuntimeVal> result;

      if(!evaluationStack.empty()){

        result = evaluationStack.top();

        evaluationStack.pop();
      }


/* if (result) { */
/*     std::cout << "EVAL EXPR: " << result->getType() << "  " << result->toString() << "\n"; */
/* } else { */
/*     std::cout << "EVAL EXPR: null\n"; */
/* } */


      return result;
  }



    // Error handling methods
    void Interpreter::runtimeError(const std::string& message) {
        // Implement error handling
        throw std::invalid_argument("Runtime Error: " + message);
        // You can throw an exception or handle it in another way
    }


void Interpreter::printStack() {
    std::stack<std::shared_ptr<RuntimeVal>> tempStack = evaluationStack; // Make a copy to iterate

    std::cout << "\nStack contents:" << std::endl;
    while (!tempStack.empty()) {
      std::shared_ptr<RuntimeVal> val = tempStack.top();
        tempStack.pop();
        std::cout << val->toString() << std::endl; // Assuming RuntimeVal has toString
    }

    std::cout << "\nEND Stack contents\n" << std::endl;
}

void Interpreter::printEnv(){
  for (const auto& pair : currentScope()->variables) {
        std::cout << "Var: " << pair.first << ", Value: " << pair.second->toString() << std::endl;
    }
}


