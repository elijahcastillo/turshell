
#include "include/Interpretor.h"
#include "include/AST_Types.h"
#include "include/Enviorment.h"
#include "include/Runtime.h"
#include "include/TurshellLog.h"
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

    const char* what () const throw() {
        return msg;
    }
};

class TurshellBreak : public std::exception {

    public:
    const char* msg;

    TurshellBreak(const char* msg) : msg(msg) {};

    const char* what () const throw() {
        return msg;
    }
};




std::string extractInnerTypeFromArrayType(const std::string& arrayType) {
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



  bool Interpreter::isStructType(const std::string& name) {
      auto it = structTable.find(name);
      if (it != structTable.end()) {
          return true;
      }
      return false;
  }


bool startsWith(const std::string& fullString, const std::string& starting) {
    // Check if the last occurrence is at the start of the string
    return fullString.rfind(starting, 0) == 0;
}

std::string Interpreter::getStructPropertyType(std::string structType, std::string propertyName){

        if(!isStructType(structType)){
          runtimeError("Cannot get struct property type on non struct type");
        }

        StructDeclInfo structInfo = structTable[structType];

        if (structInfo.properties.find(propertyName) == structInfo.properties.end()) {
            runtimeError("Unknown property in struct: " + propertyName);
        }

        std::string propertyType = structInfo.properties[propertyName];

        return  propertyType;
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
        /* node.left->accept(*this); */
        /* node.right->accept(*this); */

        std::shared_ptr<RuntimeVal> right = evaluateExpression(node.right);
        std::shared_ptr<RuntimeVal> left = evaluateExpression(node.left);

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
                   node.op == ">=" || node.op == "<=" || node.op == "!=") {
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
      auto value = std::make_shared<IntValue>(node.value);

      TurshellLog::Log("IntLiteralNode created at " + pointerAddrToString(value.get()) + " of value " + std::to_string(node.value), TurshellLog::LOG_MEMORY_INFO);
      evaluationStack.push(value);

      //evaluationStack.push(std::make_shared<IntValue>(node.value));
    }

    void Interpreter::visit(FloatLiteralNode& node) {
      evaluationStack.push(std::make_shared<FloatValue>(node.value));
    }

    void Interpreter::visit(StringLiteralNode& node) {
      evaluationStack.push(std::make_shared<StringValue>(node.value));
    }


void Interpreter::handleArrayValidation(std::shared_ptr<RuntimeVal> arr, const std::string& expectedArrayType) {
    if (auto arrayVal = dynamic_cast<ArrayValue*>(arr.get())) {
        std::string expectedTypeInArray = extractInnerTypeFromArrayType(expectedArrayType);

        // Check if the expected type inside the array is also an array
        if (startsWith(expectedTypeInArray, "array<")) {
            // Recursively validate each element in the array
            for (auto& element : arrayVal->elements) {
                handleArrayValidation(element, expectedTypeInArray);
            }
        } else if (isStructType(expectedTypeInArray)) {
            // Handle struct type validation
            for (auto& element : arrayVal->elements) {
                if (auto structVal = dynamic_cast<StructValue*>(element.get())) {
                    if (!validateAndSetStructType(element, expectedTypeInArray)) {
                        runtimeError("All structs in array must be of same type");
                    }
                } else {
                    runtimeError("Trying to assign value of type '" + expectedTypeInArray + "' to a struct type");
                }
            }
        } else {
            // Validate primitive types in array
            for (auto& element : arrayVal->elements) {
                if (element->getType() != expectedTypeInArray) {
                    runtimeError("Type mismatch in array elements");
                }
            }
        }

        arrayVal->type = "array<" + expectedTypeInArray + ">";
        arrayVal->elementType = expectedTypeInArray;
    } else {
        runtimeError("Expected type of ArrayValue in handleArrayValidation");
    }
}


void Interpreter::visit(ChainedAssignmentNode& node) {
  std::shared_ptr<RuntimeVal> valueToAssign = evaluateExpression(node.value);
  /* std::cout << "Chained Assigment value to assign: " <<  valueToAssign << " of value " << valueToAssign->toString() << "\n"; */

    auto chainedAccess = dynamic_cast<ChainedAccessNode*>(node.accesses);
    if(chainedAccess == nullptr){
      runtimeError("Chain Assignment .value must be good");
    }

    //===============================================================
      std::shared_ptr<RuntimeVal> first = evaluateExpression(chainedAccess->accesses[0]);

      StructValue* firstStructValue = dynamic_cast<StructValue*>(first.get());
      ArrayValue* firstArrayValue = dynamic_cast<ArrayValue*>(first.get());

    if (!firstStructValue && !firstArrayValue && first->type != "string") {
          runtimeError("First node variable in the access chain is not a valid type of array or struct");
      }


      //Push to stack to be used by the other nodes in chain
      evaluationStack.push(first);

      if(chainedAccess->accesses.size() <= 1){
        runtimeError("Really bad chain thing");
      }



      for (size_t i = 1; i < chainedAccess->accesses.size(); ++i) {
          chainedAccess->accesses[i]->accept(*this); // This will push the result of each access onto the stack
      }


    //===============================================================
    // The target should now be at the top of the evaluation stack
    ASTNode* target = chainedAccess->accesses[chainedAccess->accesses.size() - 1];

    // The previous value in the stack should be the object or array that contains the target
    evaluationStack.pop();
    std::shared_ptr<RuntimeVal> container = evaluationStack.top();
    evaluationStack.pop();



    // Now you need to determine what the target is and assign the value appropriately
    if (auto structVal = dynamic_cast<StructValue*>(container.get())) {

      std::string propName = static_cast<PropertyAccessNode*>(target)->propertyName;
      std::string propType = getStructPropertyType(structVal->getType(), propName);

      //Handle array assigment to stuct property
      if(startsWith(propType, "array<")){
        handleArrayValidation(valueToAssign, propType);
      }

      //Handle struct assigment to struct property
      if(isStructType(propType)){
        validateAndSetStructType(valueToAssign, propType);
      }

      if(valueToAssign->getType() != propType){
        runtimeError("Chained Assigment type mismatch for struct property");
      }



      structVal->setProperty(propName, valueToAssign);


    
    } 
    else if (auto arrayVal = dynamic_cast<ArrayValue*>(container.get())) {
        auto access = static_cast<ArrayAccessNode*>(target);
        std::shared_ptr<RuntimeVal> indexVal = evaluateExpression(access->index);

        if (indexVal->getType() != "int") {
            runtimeError("Array index must be an integer");
            return;
        }

        int index = std::static_pointer_cast<IntValue>(indexVal)->getValue();


        if(isStructType(arrayVal->elementType)){
          if(!validateAndSetStructType(valueToAssign, arrayVal->elementType)){
              runtimeError("All structs in array literal must be of same type");
          }
        }

        if(startsWith(arrayVal->elementType, "array<")){
          handleArrayValidation(valueToAssign, arrayVal->elementType);
        }


        if(arrayVal->elementType != valueToAssign->getType()){
          runtimeError("Chained Assigment array index type mismatch");
        }


        // For an array element
        arrayVal->setElement(index, valueToAssign);

    } else if (auto string = dynamic_cast<StringValue*>(container.get())) {
        auto access = static_cast<ArrayAccessNode*>(target);
        std::shared_ptr<RuntimeVal> indexVal = evaluateExpression(access->index);

        if (indexVal->getType() != "int") {
            runtimeError("Array index must be an integer");
            return;
        }

        int index = std::static_pointer_cast<IntValue>(indexVal)->getValue();


        if (valueToAssign->getType() != "string" || valueToAssign->toString().length() != 1) {
            runtimeError("Can only assign a single character to a string index");
            return;
        }
        if (index < 0 || index >= string->value.size()) {
            runtimeError("String index out of range");
            return;
        }

        string->value[index] = valueToAssign->toString()[0];

    } 
    else {
        runtimeError("Invalid target for assignment");
    }
}


void Interpreter::visit(ChainedAccessNode& node) {
    // Iterate over the accesses and resolve each step
    std::shared_ptr<RuntimeVal> currentVal = nullptr;



    std::shared_ptr<RuntimeVal> first = evaluateExpression(node.accesses[0]);

    StructValue* firstStructValue = dynamic_cast<StructValue*>(first.get());
    ArrayValue* firstArrayValue = dynamic_cast<ArrayValue*>(first.get());


    if (!firstStructValue && !firstArrayValue && first->type != "string") {
        runtimeError("First node variable in the access chain is not a valid type of array or struct or string");
    }




    //Push to stack to be used by the other nodes in chain
    evaluationStack.push(first);

    if(node.accesses.size() <= 1){
      runtimeError("Really bad chain thing");
    }



    for (size_t i = 1; i < node.accesses.size(); ++i) {
        node.accesses[i]->accept(*this); // This will push the result of each access onto the stack
    }

    if(evaluationStack.empty()){
      return;
    }

    currentVal = evaluationStack.top();
    evaluationStack.push(currentVal); // Push the final result onto the stack
}


void Interpreter::visit(PropertyAccessNode& node) {
    // Assuming the object (struct) is already on the stack as a result of previous operations
    std::shared_ptr<RuntimeVal> structVal = evaluationStack.top();
    /* evaluationStack.pop(); */

    // Check if the structVal is indeed a struct and perform the property access
    if (auto structObj = dynamic_cast<StructValue*>(structVal.get())) {
        evaluationStack.push(structObj->getProperty(node.propertyName));
    } else {
        runtimeError("Trying to access a property of a non-struct type");
    }
}


void Interpreter::visit(ArrayAccessNode& node) {
    // Evaluate the index expression and ensure it's an integer
    std::shared_ptr<RuntimeVal> indexVal = evaluateExpression(node.index);

    if (indexVal->getType() != "int") {
        runtimeError("Array index must be an integer");
        return;
    }
    int index = std::static_pointer_cast<IntValue>(indexVal)->getValue();

    // Assuming the array is already on the stack as a result of previous operations
    std::shared_ptr<RuntimeVal> arrayVal = evaluationStack.top();
    /* evaluationStack.pop(); */

    // Check if the arrayVal is indeed an array and perform the access
    if (ArrayValue* array = dynamic_cast<ArrayValue*>(arrayVal.get())) {
        if (index < 0 || index >= array->elements.size()) {
            runtimeError("Array index out of bounds3");
            return;
        }
        evaluationStack.push(array->getElement(index));
    } else if (StringValue* string = dynamic_cast<StringValue*>(arrayVal.get())){
        if (index < 0 || index >= string->value.size()) {
            runtimeError("String index out of range in array access node");
        }
        char charAtIdx = string->value[index];
        evaluationStack.push(std::make_shared<StringValue>(std::string(1, charAtIdx)));
        return;

    } else {
        runtimeError("Trying to index a non-array type");
    }
}

//============================







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
          /* std::cout << "Pushing struct into array arr lit\n"; */
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

    TurshellLog::Log("ArrayLiteralNode created at " +  pointerAddrToString(arrayVal.get()) + " of value " + arrayVal->toString(), TurshellLog::LOG_MEMORY_INFO);


    // Push the populated ArrayValue onto the evaluation stack
    evaluationStack.push(arrayVal);
}


void Interpreter::visit(BinaryLiteralNode& node) {
  evaluationStack.push(node.value ? std::make_shared<BoolValue>(true) : std::make_shared<BoolValue>(false));

}





//Variable Delcartion +_+_+_+_+_+_+_+
void Interpreter::visit(VariableDeclarationNode& node) {

    TurshellLog::Log("Varaible Declaration Started for variable '"  + node.variableName + "'", TurshellLog::LOG_INFO);

    //Get value to initilize varible with
    std::shared_ptr<RuntimeVal> value = evaluateExpression(node.initializer);


    TurshellLog::Log("Variable declaration of '" + node.variableName +  "' : initilizer evaluated at: " + pointerAddrToString(value.get()) + " of value " + value->toString(), TurshellLog::LOG_MEMORY_INFO);


    // Handle arrays
    if (auto arrayVal = dynamic_cast<ArrayValue*>(value.get())) {
      handleArrayValidation(value, node.variableType);
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


      TurshellLog::Log("Varaible Assignment Started for variable '"  + node.variableName + "'", TurshellLog::LOG_INFO);

      //Needed for type checking of new value
      std::shared_ptr<RuntimeVal> assignTo = currentScope()->getVariable(node.variableName);

      //New value to assign to variable
      std::shared_ptr<RuntimeVal> value = evaluateExpression(node.value);



    TurshellLog::Log("Varible Assignment: assigning value at " + pointerAddrToString(value.get()) + " of " + value->toString() + " to " + node.variableName + " at " + pointerAddrToString(assignTo.get()) + " with prev value of " + assignTo->toString() , TurshellLog::LOG_MEMORY_INFO);


    // Handle array literals with struct initializer lists
    if (auto arrayVal = dynamic_cast<ArrayValue*>(value.get())) {
      handleArrayValidation(value, assignTo->getType());
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

          enterNewScope();

          try{

            evaluateExpression(node.body);

          } catch(TurshellBreak e) {
            exitCurrentScope();
            break;
          }
          
          //Run code inside of {}
          exitCurrentScope();

          //Recheck condition
          condition = evaluateExpression(node.condition);

          if(condition->getType() != "bool"){
            runtimeError("Cannot evaluate 'while' condition of on non boolean type");
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



void Interpreter::visit(StructMethodCallNode& node) {

  std::shared_ptr<RuntimeVal> structValue = evaluationStack.top();
  auto structRuntime = dynamic_cast<StructValue*>(structValue.get());
  if(structRuntime == nullptr){
    runtimeError("Cannot call a struct method on a on struct type");
  }

  FunctionCallNode* funcCall = dynamic_cast<FunctionCallNode*>(node.functionCall);

  //Set self as varaible
  currentScope()->setVariable("SECRET_SELF", structValue, VariableSettings::Declaration);

  //Grab self variable
  //Insert struct parameter at begining
  ASTNode* selfParam = new VariableExpressionNode("SECRET_SELF");
  funcCall->arguments.insert(funcCall->arguments.begin(), selfParam);

  //Push struct name to eval stack to funcCall node can get it
  evaluationStack.push(std::make_shared<StringValue>(structValue->getType()));

  //Call method
  visit(*funcCall);

}

void Interpreter::visit(StructMethodDeclarationNode& node) {

  if(!isStructType(node.structName)){
    runtimeError("Struct type dosent exist '" + node.structName +"'");
  }  

  auto structInfo = structTable[node.structName];

  if (structInfo.methods.find(node.methodName) != structInfo.methods.end()) {
      // Function with the same name already exists
      runtimeError("Struct method'" + node.methodName + "' is already declared.");
  }

  auto funcDeclNode = dynamic_cast<FunctionDeclarationNode*>(node.methodDeclaration);

  /* std::cout << "Setting Method Declartion " << node.methodName << " to " << node.structName << std::endl; */

  structTable[node.structName].methods[node.methodName] = funcDeclNode;


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

        /* std::cout << "Inside validate struct\n"; */
        auto structInfo = structTable[expectedType];
        /* std::cout << "Got Struct Info: " << structInfo.structName << "\n"; */

        if (structValue->properties.size() != structInfo.numProperties) {
            runtimeError("Struct initializer size does not match definition");
            return false;
        }

        /* std::cout << "Passed struct validate #prop\n"; */

        for (const auto& prop : structValue->properties) {
            if (structInfo.properties.find(prop.first) == structInfo.properties.end()) {
                runtimeError("Unknown property in struct: " + prop.first);
                return false;
            }


          /* std::cout << "Passed struct validate prop exists in struct\n"; */

            const std::string& expectedPropType = structInfo.properties[prop.first];

            

            // If property is a struct, recursively validate and set its type
            if (auto nestedStructVal = std::dynamic_pointer_cast<StructValue>(prop.second)) {
                if (!validateAndSetStructType(nestedStructVal, expectedPropType)) {
                    return false;
                }

            } else if (auto arrayVal = dynamic_cast<ArrayValue*>(prop.second.get())) {

                handleArrayValidation(prop.second, expectedPropType);


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



    void Interpreter::visit(StructPropertyAccessNode& node) {};
    void Interpreter::visit(StructPropertyAssignmentNode& node) {};



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

        if(node.isStructMethod){
          /* std::cout << "isStructMethod = true\n"; */

          //Get struct type
          std::string structType = evaluationStack.top()->toString();


          evaluationStack.pop();
          if(!isStructType(structType)){
            runtimeError("Struct type does not exist for method");
          }

          StructDeclInfo structDecl = structTable[structType]; 



          
          // Check if the function is declared by user
          if (structDecl.methods.find(node.functionName) != structDecl.methods.end()) {
            functionDecl = structDecl.methods[node.functionName];
          }

        } else {

            // Check if the function is declared by user
            if (functionTable.find(node.functionName) != functionTable.end()) {
              functionDecl = functionTable[node.functionName];
            }

            // Check if the function is a native function
            if (nativeFunctions.find(node.functionName) != nativeFunctions.end()) {
              isNativeFunction = true;
            }

        }


        /* std::cout << "FunctionDecl = " << (functionDecl == nullptr) << " \n"; */

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
          TurshellLog::Log("Started Native Function!", TurshellLog::LOG_INFO);

          std::vector<std::shared_ptr<RuntimeVal>> arguments;
          for(int i = 0; i < node.arguments.size(); i++){
            arguments.push_back(evaluateExpression(node.arguments[i]));
          }
          std::shared_ptr<RuntimeVal> returnValue = callNativeFunction(node.functionName, arguments);
          if(returnValue != nullptr){
            evaluationStack.push(returnValue);
          }
          return;
        }


        enterNewScope();

        // Bind arguments to parameters in the new local scope
        for (size_t i = 0; i < node.arguments.size(); ++i) {
            
            ParameterNode* declParam = dynamic_cast<ParameterNode*>(functionDecl->parameters[i]);
            if(declParam == nullptr){
              runtimeError("Function decl parameters not valid");
            }

            


            std::string paramName = declParam->name;
            std::shared_ptr<RuntimeVal> argValue = evaluateExpression(node.arguments[i]);

            //Check type of parameters to fuction declaration
            if(argValue->getType() != declParam->type){
              runtimeError("Invalid function parameter type of '" + argValue->getType() + "' expected '" + declParam->type + "' in function '" + functionDecl->functionName + "'");
              return;
            }

             

            /* std::cout << "Setting param for function: " << paramName << " " << argValue << "\n"; */
            currentScope()->setVariable(paramName, argValue, VariableSettings::Declaration);

        }



    // Remember the current stack depth
    int initialStackDepth = envStack.size();


        //Evalute body of function
        bool didReturn = false;
        std::shared_ptr<RuntimeVal> returnValue = nullptr;

          try{

            evaluateExpression(functionDecl->body);


          } catch(TurshellReturn e) {

              //Returns env stack back to orignal state
              //Helps when you return from a function inside of another block
              //like an if statment where since you returned the scope is never closed
              while (envStack.size() > initialStackDepth) {
                  exitCurrentScope();
              }

            //End Body execution early
            returnValue = e.returnVal;
            didReturn = true;




          }







        // Pop the local scope from the stack
        exitCurrentScope();


        /* std::cout << "DID RETURN " << didReturn  << (returnValue == nullptr) << "\n"; */

        if(functionDecl->returnType == "void" && returnValue != nullptr){
          runtimeError("Function of return type void cannot return a value");
        }


        if(didReturn && returnValue != nullptr){

          //Handle return of array
          if(startsWith(functionDecl->returnType, "array<")){
            handleArrayValidation(returnValue, functionDecl->returnType);
          }

          //Handle return of struct
          if(isStructType(functionDecl->returnType)){
            validateAndSetStructType(returnValue, functionDecl->returnType);
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

      TurshellLog::Log("VaribleExpressionNode: getting variable " + node.variableName, TurshellLog::LOG_INFO);
      TurshellLog::Log("VaribleExpressionNode, getting variable " + node.variableName + " located at addr " + pointerAddrToString(value.get()) + " of value " + value->toString(), TurshellLog::LOG_MEMORY_INFO);

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


    void Interpreter::visit(BreakStatementNode& node) {

        throw TurshellBreak("Break Error - Can only use 'break' inside of while loop");
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

void Interpreter::printEnvParent(){
  std::cout << "ParentEnv: \n";
  if(currentScope()->parent){
    for (const auto& pair : currentScope()->parent->variables) {
          std::cout << "Var: " << pair.first << ", Value: " << pair.second->toString() << std::endl;
      }

  }

}


