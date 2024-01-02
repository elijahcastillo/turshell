
#include "include/Interpretor.h"
#include "include/Enviorment.h"
#include "include/Runtime.h"
#include <string>



//HelperFunctions and Classes
class TurshellReturn : public std::exception {

    public:
    RuntimeVal returnVal;
    const char* msg;

    TurshellReturn(RuntimeVal returnVal, const char* msg) : returnVal(returnVal), msg(msg) {};

    const char * what () {
        return msg;
    }
};


bool checkType(std::string type, RuntimeVal value) {
    if ((type == "int" && !value.isInt()) ||
        (type == "bool" && !value.isBool()) ||
        (type == "string" && !value.isString())) {
      return false;
    }

    return true;
}



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
          //String Concat
          if(left.isString() && right.isString()){
            evaluationStack.push(RuntimeVal(left.getString() + right.getString()));
          }

          //Integer
          if(left.isInt() && right.isInt()){
            evaluationStack.push(RuntimeVal(left.getInt() + right.getInt()));
          }

        } else if (node.op == "-"){
          evaluationStack.push(RuntimeVal(left.getInt() - right.getInt()));
        } else if (node.op == "*"){
          evaluationStack.push(RuntimeVal(left.getInt() * right.getInt()));
        } else if (node.op == "/"){
          evaluationStack.push(RuntimeVal(left.getInt() / right.getInt()));
        } else if (node.op == "%"){
          evaluationStack.push(RuntimeVal(left.getInt() % right.getInt()));
        }


        if(node.op == "=="){

          if(left.isString() && right.isString()){
            if(left.getString() == right.getString()){
              return evaluationStack.push(RuntimeVal(true));
            } else {

              return evaluationStack.push(RuntimeVal(false));
            }

          }

          if(left.isInt() && right.isInt()){
            //Integer
            if(left.getInt() == right.getInt()){
              return evaluationStack.push(RuntimeVal(true));
            } else {
              return evaluationStack.push(RuntimeVal(false));
            }
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
          if(left.getInt() < right.getInt()){
            return evaluationStack.push(RuntimeVal(true));
          } else {
            return evaluationStack.push(RuntimeVal(false));
          }
        }


        if(node.op == "<="){
          if(left.getInt() <= right.getInt()){
            return evaluationStack.push(RuntimeVal(true));
          } else {
            return evaluationStack.push(RuntimeVal(false));
          }
        }


        if(node.op == ">="){
          if(left.getInt() >= right.getInt()){
            return evaluationStack.push(RuntimeVal(true));
          } else {
            return evaluationStack.push(RuntimeVal(false));
          }
        }


    }

    void Interpreter::visit(IntLiteralNode& node) {
      evaluationStack.push(RuntimeVal(node.value));

    }

    void Interpreter::visit(StringLiteralNode& node) {
      evaluationStack.push(RuntimeVal(node.value));

    }

    void Interpreter::visit(VariableDeclarationNode& node) {
      RuntimeVal value = evaluateExpression(node.initializer);

      if(!checkType(node.variableType, value)){
          runtimeError("Type mismatch in variable declaration for " + node.variableName);
          return;
      }

        currentScope()->setVariable(node.variableName, value, VariableSettings::Declaration);
    }

    // VariableAssignmentNode
    void Interpreter::visit(VariableAssignmentNode& node) {
      RuntimeVal value = evaluateExpression(node.value);

      /* std::cout << "Setting variable " << node.variableName << " to " << value.getInt() << "\n"; */
      currentScope()->setVariable(node.variableName, value, VariableSettings::Assignment);
      
    }

    void Interpreter::visit(WhileStatementNode& node) {
        // Implement while loop logic

        RuntimeVal condition = evaluateExpression(node.condition);

        while(condition.getBool()  == true){
          
          //Run code inside of {}
          enterNewScope();
          evaluateExpression(node.body);
          exitCurrentScope();

          //Recheck condition
          condition = evaluateExpression(node.condition);
          
        }
    }

    void Interpreter::visit(IfStatementNode& node) {
        // Implement if statement logic
        RuntimeVal condition = evaluateExpression(node.condition);

        //Run code inside {}
        if(condition.getBool() == true){
          enterNewScope();
          evaluateExpression(node.thenBranch);
          exitCurrentScope();
        }

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

      //Handle STD functions
      if(node.functionName == "print"){
        //Eval first argument
        RuntimeVal value = evaluateExpression(node.arguments[0]);

        if(value.isInt()){
          std::cout << value.getInt() << "\n";
          return;
        }

        if (value.isString()){
          std::cout << value.getString() << "\n";
          return;
        }

        runtimeError("Cannot print out vairable of unknown type");


      }


        // Check if the function is declared
        if (functionTable.find(node.functionName) == functionTable.end()) {
            runtimeError("Function '" + node.functionName + "' is not declared.");
        }

        // Get the function declaration
        FunctionDeclarationNode* functionDecl = functionTable[node.functionName];

        // Check the number of arguments
        if (node.arguments.size() != functionDecl->parameters.size()) {
            runtimeError("Function '" + node.functionName + "' called with the wrong number of arguments. Got " + std::to_string(node.arguments.size()) + " expected " + std::to_string(functionDecl->parameters.size()));
        }


        // Create a new local scope for the function call parameters, since visit block node creates its own scope


        enterNewScope();

        // Bind arguments to parameters in the new local scope
        for (size_t i = 0; i < node.arguments.size(); ++i) {
            
            ParameterNode* declParam = dynamic_cast<ParameterNode*>(functionDecl->parameters[i]);
            ParameterNode* callParam = dynamic_cast<ParameterNode*>(node.arguments[i]);

            std::cout << "Decl Name: " << declParam->name << "\n";
            std::cout << "call v: " << evaluateExpression(node.arguments[i]).getInt() << "\n";

            std::string paramName = declParam->name;
            RuntimeVal argValue = evaluateExpression(node.arguments[i]);
            currentScope()->setVariable(paramName, argValue, VariableSettings::Declaration);
        }


        //Evalute body of function
        bool didReturn = false;
        RuntimeVal returnValue;

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
          std::cout << "Returned call: " << returnValue.getInt() << "\n";

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
      RuntimeVal value = currentScope()->getVariable(node.variableName);
      evaluationStack.push(value);
    }

    void Interpreter::visit(ReturnStatementNode& node) {
           // Evaluate the expression to be returned
        RuntimeVal returnValue;
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


