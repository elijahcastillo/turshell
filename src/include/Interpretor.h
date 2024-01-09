
#pragma once
#include "AST_Types.h"
#include "Visitors.h"
#include "Enviorment.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <stack>



struct StructDeclInfo{
  std::string structName;
  size_t numProperties;
  std::unordered_map<std::string, std::string> properties;

  StructDeclInfo(std::string structName, size_t numProperties): structName(structName), numProperties(numProperties) {
  };

  StructDeclInfo(){
  }

  void addProperty(std::string propName, std::string propType){
    properties[propName] = propType;
  }
};

class Interpreter : public Visitor {
    // Environment to store variable values, function definitions, etc.

    std::stack<std::shared_ptr<Environment>> envStack; //Stack of Enviornment scopes
    std::stack<std::shared_ptr<RuntimeVal>> evaluationStack; //Stack to help durring calculations
    std::unordered_map<std::string, FunctionDeclarationNode*> functionTable; //User defined functions
    std::unordered_map<std::string, StructDeclInfo> structTable;
    std::unordered_map<std::string, std::function<std::shared_ptr<RuntimeVal>(Interpreter&, std::vector<std::shared_ptr<RuntimeVal>>&)>> nativeFunctions;





public:

    Interpreter(){
      envStack.push(std::make_shared<Environment>());
    }

    void registerNativeFunction(const std::string& name, std::function<  std::shared_ptr<RuntimeVal>(  Interpreter&, std::vector<std::shared_ptr<RuntimeVal>>&  )  > func); 
    std::shared_ptr<RuntimeVal> callNativeFunction(const std::string& name, std::vector<std::shared_ptr<RuntimeVal>>& args); 

  bool isStructType(const std::string& name); 




bool validateAndSetStructType(std::shared_ptr<RuntimeVal> structVal, const std::string& expectedType);

void handleArrayValidation(std::shared_ptr<RuntimeVal> arr, const std::string& expectedArrayType);

std::string getStructPropertyType(std::string structType, std::string propertyName);



void printEnvParent();

    void visit(ProgramNode& node) override;

    void visit(BinaryExpressionNode& node) override;
    void visit(UnaryExpressionNode& node) override;
    void visit(LogicalOperatorNode& node) override;

    void visit(IntLiteralNode& node) override ;
    void visit(FloatLiteralNode& node) override ;
    void visit(StringLiteralNode& node) override;
    void visit(BinaryLiteralNode& node) override;
    void visit(ArrayLiteralNode& node) override;


    void visit(ArrayAccessNode& node) override;
    void visit(PropertyAccessNode& node) override;
    void visit(ChainedAccessNode& node) override;
    void visit(ChainedAssignmentNode& node) override;




    void visit(StructDeclarationNode& node) override;
    void visit(StructInitalizerListNode& node) override;
    void visit(StructPropertyAccessNode& node) override;
    void visit(StructPropertyAssignmentNode& node) override;


    void visit(VariableDeclarationNode& node) override;

    void visit(VariableAssignmentNode& node) override;

    void visit(WhileStatementNode& node) override ;

    void visit(IfStatementNode& node) override;

    void visit(FunctionDeclarationNode& node) override ;

    void visit(BlockNode& node) override;

    void visit(VariableExpressionNode& node) override;

    void visit(ReturnStatementNode& node) override;
    void visit(BreakStatementNode& node) override;

    void visit(ParameterNode& node) override;
    void visit(FunctionCallNode& node) override;


    std::shared_ptr<Environment> currentScope();

    void enterNewScope();

    void exitCurrentScope();
    // Helper methods
    std::shared_ptr<RuntimeVal> evaluateExpression(ASTNode* node); 

    // Error handling methods
    void runtimeError(const std::string& message);

    void printStack(); // Method to print the stack contents
    void printEnv(); // Method to print the stack contents
                       //

    // Add more helper and utility methods as needed...
};
