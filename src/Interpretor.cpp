
#include "include/Interpretor.h"
#include "include/AST_Types.h"
#include "include/Enviorment.h"
#include "include/Runtime.h"
#include <iostream>
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

  bool Interpreter::isStructType(const std::string& name) {
      auto it = structTable.find(name);
      if (it != structTable.end()) {
          return true;
      }
      return false;
  }



bool isPrimitiveType(const std::string& type) {
    return type == "int" || type == "string" || type == "bool" || type == "float";
}



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
    return nullptr;
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

    try {
        if (node.op == "+") {
            evaluationStack.push(RuntimeOps::add(left, right));
        } else if (node.op == "-") {
            evaluationStack.push(RuntimeOps::subtract(left, right));
        } else if (node.op == "*") {
            evaluationStack.push(RuntimeOps::multiply(left, right));
        } else if (node.op == "/") {
            evaluationStack.push(RuntimeOps::divide(left, right));
        } else if (node.op == "%"){
          evaluationStack.push(RuntimeOps::mod(left, right));
        } else if (node.op == "==" || node.op == ">" || node.op == "<" || 
                   node.op == ">=" || node.op == "<=") {
            evaluationStack.push(RuntimeOps::compare(left, right, node.op));
        } else {
            throw std::runtime_error("Unsupported operation in binary expression");
        }
    } catch (const std::runtime_error& e) {
        runtimeError(e.what());
    }
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

    void Interpreter::visit(FloatLiteralNode& node) {
      evaluationStack.push(std::make_shared<FloatValue>(node.value));
    }

    void Interpreter::visit(StringLiteralNode& node) {
      evaluationStack.push(std::make_shared<StringValue>(node.value));
    }


void Interpreter::visit(ArrayAccessNode& node) {
    std::shared_ptr<RuntimeVal> accessIndex = evaluateExpression(node.value);
    if (accessIndex->getType() != "int") {
        runtimeError("Array access index must be an int");
    }
    auto accessIndexValue = static_cast<IntValue*>(accessIndex.get())->value;

    std::shared_ptr<RuntimeVal> runtimeValue = currentScope()->getVariable(node.identifier);

    // Check if the variable is an ArrayValue
    ArrayValue* array = dynamic_cast<ArrayValue*>(runtimeValue.get());
    if (array != nullptr) {
        if (accessIndexValue < 0 || accessIndexValue >= array->elements.size()) {
            runtimeError("Array access out of range");
        }
        evaluationStack.push(array->elements[accessIndexValue]);
        return;
    }

    // Check if the variable is a StringValue
    StringValue* stringVal = dynamic_cast<StringValue*>(runtimeValue.get());
    if (stringVal != nullptr) {
        const std::string& str = stringVal->getValue();
        if (accessIndexValue < 0 || accessIndexValue >= str.size()) {
            runtimeError("String index out of range");
        }
        char charAtIdx = str[accessIndexValue];
        evaluationStack.push(std::make_shared<StringValue>(std::string(1, charAtIdx)));
        return;
    }

    // If it's neither an array nor a string, throw an error
    runtimeError("Can only use array access on arrays and strings");
}





void Interpreter::visit(ArrayLiteralNode& node) {
    // Assuming that you have a way to determine the type of elements in the array,
    // for instance, based on the type of the first element or a predefined type.
    // This example uses a generic type for simplicity.
    std::string elementTypes = "unknown"; 



    std::vector<std::shared_ptr<RuntimeVal>> arrayValues;

    // Evaluate each element in the array literal and add it to the arrayVal
    for (auto& element : node.values) {
        std::shared_ptr<RuntimeVal> value = evaluateExpression(element);

        //We dont know the type if its a struct, need more context
        if(auto structCast = dynamic_cast<StructValue*>(value.get())){
          arrayValues.push_back(value);
          continue;
        }

        //Check types
        if(elementTypes == "unknown"){
          elementTypes = value->getType();
        } else {
          if(value->getType() != elementTypes){
              runtimeError("All types in an array literal must me the same");
          }
        }

        // Add the evaluated element to the array
        arrayValues.push_back(value);
    }

    // Create an instance of ArrayValue to store the elements
    auto arrayVal = std::make_shared<ArrayValue>(elementTypes, arrayValues);

    // Push the populated ArrayValue onto the evaluation stack
    evaluationStack.push(arrayVal);
}


void Interpreter::visit(BinaryLiteralNode& node) {
  evaluationStack.push(node.value ? std::make_shared<BoolValue>(true) : std::make_shared<BoolValue>(false));

}


std::string extractStructTypeFromArrayType(const std::string& arrayType) {
    size_t start = arrayType.find('<');
    size_t end = arrayType.find('>');

    if (start == std::string::npos || end == std::string::npos || end <= start) {
        throw std::runtime_error("Invalid array type format: " + arrayType);
    }

    return arrayType.substr(start + 1, end - start - 1);
}

    void Interpreter::visit(VariableDeclarationNode& node) {


      std::shared_ptr<RuntimeVal> value = evaluateExpression(node.initializer);

    // Handle array literals with struct initializer lists
    if (auto arrayVal = dynamic_cast<ArrayValue*>(value.get())) {
        if (node.variableType.rfind("array<", 0) == 0) {
            // Extract struct type from the variable type (e.g., "array<MyStruct>")
            std::string structType = extractStructTypeFromArrayType(node.variableType);

            // Set the type of each struct in the array
            for (auto& element : arrayVal->elements) {
                if (auto structVal = dynamic_cast<StructValue*>(element.get())) {
                    if(!validateAndSetStructType(element, structType)){
                      runtimeError("All structs in array literal must be of same type");
                    }
                }
            }


            // Update the element type of the array
            arrayVal->type = "array<" + structType + ">";
            arrayVal->elementType = structType;

        }
    }



    if (isStructType(node.variableType) && !validateAndSetStructType(value, node.variableType)) {
        runtimeError("Struct type mismatch in variable declaration for " + node.variableName);
        return;
    }
       

      if(!(node.variableType == value->getType())){
        
          runtimeError("Type mismatch in variable declaration for " + node.variableName + " of " + node.variableType + " and " + value->getType());
          return;
      }


        currentScope()->setVariable(node.variableName, value, VariableSettings::Declaration);
    }




    // ==========VariableAssignmentNode===========
    void Interpreter::visit(VariableAssignmentNode& node) {

      if (node.index != nullptr) {
        // Evaluate the index and ensure it's an integer
        std::shared_ptr<RuntimeVal> indexVal = evaluateExpression(node.index);
        if (indexVal->getType() != "int") {
            runtimeError("Index must be an integer");
            return;
        }
        int index = static_cast<IntValue*>(indexVal.get())->getValue();

        // Retrieve the variable (array or string)
        std::shared_ptr<RuntimeVal> varVal = currentScope()->getVariable(node.variableName);

        if (ArrayValue* array = dynamic_cast<ArrayValue*>(varVal.get())) {
            // Array assignment
            std::shared_ptr<RuntimeVal> value = evaluateExpression(node.value);
            if (value->getType() != array->elementType) {
                runtimeError("Type mismatch in array assignment");
                return;
            }
            array->setElement(index, value);
            return;
        } 

        if (StringValue* string = dynamic_cast<StringValue*>(varVal.get())) {
            // String assignment
            std::shared_ptr<RuntimeVal> charVal = evaluateExpression(node.value);
            if (charVal->getType() != "string" || charVal->toString().length() != 1) {
                runtimeError("Can only assign a single character to a string index");
                return;
            }
            if (index < 0 || index >= string->value.size()) {
                runtimeError("String index out of range");
                return;
            }
            string->value[index] = charVal->toString()[0];
            return;
        } 
            
        runtimeError("Indexing not supported on the given variable type");
        
        return;
    }



      std::shared_ptr<RuntimeVal> assignTo = currentScope()->getVariable(node.variableName);
      std::shared_ptr<RuntimeVal> value = evaluateExpression(node.value);


    // Handle array literals with struct initializer lists
    if (auto arrayVal = dynamic_cast<ArrayValue*>(value.get())) {
        if (assignTo->getType().rfind("array<", 0) == 0) {
            // Extract struct type from the variable type (e.g., "array<MyStruct>")
            std::string structType = extractStructTypeFromArrayType(assignTo->getType());

            // Set the type of each struct in the array
            for (auto& element : arrayVal->elements) {
                if (auto structVal = dynamic_cast<StructValue*>(element.get())) {
                    if(!validateAndSetStructType(element, structType)){
                      runtimeError("All structs in array literal must be of same type");
                    }
                }
            }


            // Update the element type of the array
            arrayVal->type = "array<" + structType + ">";
            arrayVal->elementType = structType;

        }
    }

      //Handle Structs assigmnet
    if (isStructType(assignTo->getType()) && !validateAndSetStructType(value, assignTo->getType())) {
        runtimeError("Struct type mismatch in assignment to " + node.variableName);
        return;
    }



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
        } else {
          if(node.elseBranch != nullptr){
            enterNewScope();
            evaluateExpression(node.elseBranch);
            exitCurrentScope();
          }

        }


    }



    void Interpreter::visit(StructDeclarationNode& node) {
      //Do something Here not sure yet, its a user defined type
        if (structTable.find(node.structName) != structTable.end()) {
            // Function with the same name already exists
            runtimeError("Struct '" + node.structName + "' is already declared.");
        }
        StructDeclInfo info(node.structName, node.properties.size());
        for (auto& prop : node.properties) {
          ParameterNode* declParam = static_cast<ParameterNode*>(prop);

            info.addProperty(declParam->name, declParam->type);
        }

        structTable[node.structName] = info;
    }

void Interpreter::visit(StructInitalizerListNode& node) {
    std::string structType = ""; // This should be determined from the context or passed to the node.

    // Create a StructValue for this initializer list
    auto structValue = std::make_shared<StructValue>(structType);

    for (auto& property : node.properties) {
        VariableAssignmentNode* varAssign = dynamic_cast<VariableAssignmentNode*>(property);
        if (!varAssign) {
            runtimeError("Invalid property in struct initializer list");
            return;
        }

        std::shared_ptr<RuntimeVal> propertyValue = evaluateExpression(varAssign->value);

        // Handle nested struct initializer lists
        if (auto nestedList = dynamic_cast<StructInitalizerListNode*>(varAssign->value)) {
            std::string nestedStructType = ""; // Determine the nested struct type
            propertyValue = evaluateExpression(varAssign->value); //Nested intilizer list

        }

        structValue->setProperty(varAssign->variableName, propertyValue);
    }

    evaluationStack.push(structValue);
}


bool Interpreter::validateAndSetStructType(std::shared_ptr<RuntimeVal> structVal, const std::string& expectedType) {
    if (auto structValue = std::dynamic_pointer_cast<StructValue>(structVal)) {
        auto structInfo = structTable[expectedType];

        if (structValue->properties.size() != structInfo.numProperties) {
            runtimeError("Struct initializer size does not match definition");
            return false;
        }

        for (const auto& prop : structValue->properties) {
            if (structInfo.properties.find(prop.first) == structInfo.properties.end()) {
                runtimeError("Unknown property in struct: " + prop.first);
                return false;
            }

            const std::string& expectedPropType = structInfo.properties[prop.first];
            
            // If property is a struct, recursively validate and set its type
            if (auto nestedStructVal = std::dynamic_pointer_cast<StructValue>(prop.second)) {
                if (!validateAndSetStructType(nestedStructVal, expectedPropType)) {
                    return false;
                }
            } else if (prop.second->getType() != expectedPropType) {
                runtimeError("Type mismatch for property '" + prop.first + "' in struct");
                return false;
            }
        }

        structValue->setStructType(expectedType); // Set the correct type
        return true;
    }

    runtimeError("Expected a struct type");
    return false;
}




void Interpreter::visit(StructPropertyAssignmentNode& node) {
    // Start with the base struct
    std::shared_ptr<RuntimeVal> currentVal = currentScope()->getVariable(node.baseName);

    // Check if the base variable is indeed a struct
    if (!isStructType(currentVal->getType())) {
        runtimeError("Variable '" + node.baseName + "' is not a struct type");
        return;
    }

    // Traverse through the nested properties, but stop at the second-to-last property
    for (size_t i = 0; i < node.propertyNames.size() - 1; ++i) {
        const std::string& propertyName = node.propertyNames[i];

        // Ensure that the current value is a struct to have properties
        if (!isStructType(currentVal->getType())) {
            runtimeError("Property access on non-struct type for '" + propertyName + "'");
            return;
        }

        // Cast the current value to StructValue type to access its properties
        auto structVal = std::static_pointer_cast<StructValue>(currentVal);

        // Retrieve the next property in the chain
        try {
            currentVal = structVal->getProperty(propertyName);
        } catch (const std::runtime_error& e) {
            runtimeError("Property '" + propertyName + "' not found in struct '" + currentVal->getType() + "'");
            return;
        }
    }

    // Now, handle the final property for assignment
    const std::string& finalProperty = node.propertyNames.back();
    auto structVal = dynamic_cast<StructValue*>(currentVal.get()); //Dynamic cast with null check???
    if(structVal == nullptr){
      runtimeError("Cannot assign property of " + finalProperty + " on a non struct type"); 
    }


    // If an index is provided, handle the array assignment
    if (node.index) {
        std::shared_ptr<RuntimeVal> indexVal = evaluateExpression(node.index);
        if (indexVal->getType() != "int") {
            runtimeError("Array index must be an int");
            return;
        }
        int index = static_cast<IntValue*>(indexVal.get())->getValue();


        std::shared_ptr<RuntimeVal> propertyVal = structVal->getProperty(finalProperty);

        // Check if it's an ArrayValue
        ArrayValue* array = dynamic_cast<ArrayValue*>(propertyVal.get());
        if (array) {
            std::shared_ptr<RuntimeVal> assignedValue = evaluateExpression(node.value);
            if (array->elementType != assignedValue->getType()) {
                runtimeError("Cannot assign type '" + assignedValue->getType() + "' to array of type '" + array->elementType + "'");
                return;
            }
            array->setElement(index, assignedValue);
            return;
        }

        // Check if it's a StringValue
        StringValue* stringVal = dynamic_cast<StringValue*>(propertyVal.get());
        if (stringVal) {
            std::shared_ptr<RuntimeVal> assignedValue = evaluateExpression(node.value);
            if (assignedValue->getType() != "string" || assignedValue->toString().length() != 1) {
                runtimeError("Can only assign a single character to a string index OR type mismatch");
                return;
            }
            std::string& str = stringVal->value;
            if (index < 0 || index >= str.length()) {
                runtimeError("String index out of range");
                return;
            }
            str[index] = assignedValue->toString()[0]; // Assign the character
            return;
        }

        runtimeError(finalProperty + " is neither an array nor a string");
        return;
    }


    // Check if the value to be assigned is a struct initializer list
    if (auto initializerListNode = dynamic_cast<StructInitalizerListNode*>(node.value)) {
        // Evaluate the struct initializer list to get the struct value
        std::shared_ptr<RuntimeVal> assignedValue = evaluateExpression(node.value);

        // Get the expected type of the property
        std::string expectedType = structVal->getProperty(finalProperty)->getType();

        // Validate the struct and set its type
        if (!validateAndSetStructType(assignedValue, expectedType)) {
            runtimeError("Struct initializer list does not match the expected struct type for '" + finalProperty + "'");
            return;
        }

        // Perform the assignment
        structVal->setProperty(finalProperty, assignedValue);
    } else {
        // Evaluate the value to be assigned for non-struct assignments
        std::shared_ptr<RuntimeVal> assignedValue = evaluateExpression(node.value);

        if (assignedValue->getType() != structVal->getProperty(finalProperty)->getType()) {
            runtimeError("Cannot assign type '" + assignedValue->getType() + "' to type '" + structVal->getProperty(finalProperty)->getType() + "' of property '" + finalProperty + "'");
            return;
        }

        // Perform the assignment for non-struct types
        structVal->setProperty(finalProperty, assignedValue);
    }
}


  void Interpreter::visit(StructPropertyAccessNode& node) {
      // Start with the base struct
      std::shared_ptr<RuntimeVal> structInstance = currentScope()->getVariable(node.baseName);

      // Check if the base variable is indeed a struct
      if (!isStructType(structInstance->getType())) {
          runtimeError("Variable '" + node.baseName + "' is not a struct type");
          return;
      }

      // Traverse through the nested properties
      for (const std::string& propertyName : node.propertyNames) {
          // Ensure that the current value is a struct to have properties
          if (!isStructType(structInstance->getType())) {
              runtimeError("Property access on non-struct type for '" + propertyName + "'");
              return;
          }

          // Cast the current value to StructValue type to access its properties
          auto structVal = std::static_pointer_cast<StructValue>(structInstance);

          // Retrieve the next property in the chain
          try {
              structInstance = structInstance->getProperty(propertyName);
          } catch (const std::runtime_error& e) {
              runtimeError("Property '" + propertyName + "' not found in struct '" + structInstance->getType() + "'");
              return;
          }
      }


    // Handle the array index if present
    if (node.index != nullptr) {
        std::shared_ptr<RuntimeVal> indexVal = evaluateExpression(node.index);
        if (indexVal->getType() != "int") {
            runtimeError("Array index must be an int");
        }
        auto index = static_cast<IntValue*>(indexVal.get())->getValue();

        // Check if it's an ArrayValue
        ArrayValue* arrayVal = dynamic_cast<ArrayValue*>(structInstance.get());
        if (arrayVal != nullptr) {
            if (index < 0 || index >= arrayVal->elements.size()) {
                runtimeError("Array index out of range");
                return;
            }
            evaluationStack.push(arrayVal->getElement(index));
            return;
        }


        // Check if it's a StringValue
        StringValue* stringVal = dynamic_cast<StringValue*>(structInstance.get());
        if (stringVal != nullptr) {
            const std::string& str = stringVal->getValue();
            if (index < 0 || index >= str.length()) {
                runtimeError("String index out of range");
                return;
            }
            char charAtIdx = str[index];
            evaluationStack.push(std::make_shared<StringValue>(std::string(1, charAtIdx)));
            return;
        }


        // If it's neither an array nor a string
        runtimeError("Array access in struct is neither an array nor a string");
        return;    
    }

      // Push the final property value onto the evaluation stack
      evaluationStack.push(structInstance);
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
            /* ParameterNode* callParam = dynamic_cast<ParameterNode*>(node.arguments[i]); */

            


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
          if(functionDecl->returnType == "void" && returnValue != nullptr){
            runtimeError("Function of return type void cannot return a value");
          }

          if(returnValue->getType() != functionDecl->returnType){
            runtimeError("Invalid return type of '" + returnValue->getType() + "' expected '" + functionDecl->returnType + "' in function '" + functionDecl->functionName + "'" );
            return;
          }

          if(functionDecl->returnType != "void" && returnValue != nullptr){
            // Push the return value onto the stack
            evaluationStack.push(returnValue);
          }

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
      std::shared_ptr<RuntimeVal> returnValue = nullptr;
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
  std::cout << "CurrentEnv: \n";
  for (const auto& pair : currentScope()->variables) {
        std::cout << "Var: " << pair.first << ", Value: " << pair.second->toString() << std::endl;
    }
}


