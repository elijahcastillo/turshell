#pragma once

#include "Visitors.h"
#include "AST_Types.h"
#include <fstream>
#include <sstream>
#include <string>

class CppTranspilerVisitor : public Visitor {
    std::ofstream outputFile;
    std::stringstream mainBuffer;
    std::stringstream functionDeclarations;

    bool insideFunction = false;

public:
    CppTranspilerVisitor(const std::string& outputFileName) {
        outputFile.open(outputFileName);
    }

    ~CppTranspilerVisitor() {
        if (outputFile.is_open()) {

            // Write includes and using directives first
            outputFile << "#include <iostream>\n";
            outputFile << "#include <vector>\n";
            outputFile << "#include <string>\n";
            outputFile << "using namespace std;\n\n";

            // Write function declarations first
            outputFile << functionDeclarations.str();

            // Then write the main function
            outputFile << "int main() {\n" << mainBuffer.str() << "    return 0;\n}";

            outputFile.close();
        }
    }

    std::ostream& getBufferType(){
      if(insideFunction){
        return functionDeclarations;
      } else {
        return mainBuffer;
      }
    }

    std::string convertTurshellType(std::string type){
      return "test"; 
    }

    void visit(ProgramNode& node) override; 
    void visit(BinaryExpressionNode& node) override; 
    void visit(UnaryExpressionNode& node) override; 
    void visit(LogicalOperatorNode& node) override; 

    void visit(IntLiteralNode& node) override; 
    void visit(FloatLiteralNode& node) override; 
    void visit(StringLiteralNode& node) override; 
    void visit(BinaryLiteralNode& node) override; 
    void visit(ArrayLiteralNode& node) override; 


    void visit(ArrayAccessNode& node) override; 
    void visit(PropertyAccessNode& node) override; 
    void visit(ChainedAccessNode& node) override; 
    void visit(ChainedAssignmentNode& node) override; 

    void visit(StructMethodDeclarationNode& node) override; 
    void visit(StructMethodCallNode& node) override; 
    void visit(StructDeclarationNode& node) override; 
    void visit(StructInitalizerListNode& node) override; 
    void visit(StructPropertyAccessNode& node) override; 
    void visit(StructPropertyAssignmentNode& node) override; 

    void visit(VariableDeclarationNode& node) override; 
    void visit(VariableAssignmentNode& node) override; 

    void visit(IfStatementNode& node) override;
    void visit(WhileStatementNode& node) override;
    void visit(ForStatementNode& node) override;


    void visit(BlockNode& node) override;
    void visit(VariableExpressionNode& node) override;
    void visit(FunctionDeclarationNode& node) override;
    void visit(ParameterNode& node) override;
    void visit(ReturnStatementNode& node) override;
    void visit(BreakStatementNode& node) override;
    void visit(FunctionCallNode& node) override;


};
