#pragma once

#include "Visitors.h"
#include "AST_Types.h"
#include <fstream>
#include <map>
#include <sstream>
#include <functional>
#include <string>

class CppTranspilerVisitor : public Visitor {
    std::map<std::string, std::function<void(FunctionCallNode&)>> nativeFunctionMap;
    std::ofstream outputFile;
    std::stringstream mainBuffer;
    std::stringstream functionDeclarations;
    std::map<std::string, std::stringstream> structDeclarations;
    std::string m_currentStructName; // Add this member variable

    bool insideFunction = false;
    bool insideStructDecl = false;

    //Just need it for function call ';'
    bool isStandaloneStatement = true;
public:
    CppTranspilerVisitor(const std::string& outputFileName) {
        outputFile.open(outputFileName);
        setupNativeFunctions();
    }

    ~CppTranspilerVisitor() {
        if (outputFile.is_open()) {

            // Write includes and using directives first
            outputFile << "#include <iostream>\n";
            outputFile << "#include <vector>\n";
            outputFile << "#include <string>\n";
            outputFile << "using namespace std;\n\n";


           // Write struct declarations
            for (const auto& pair : structDeclarations) {
                outputFile << "struct " << pair.first << " {\n";
                outputFile << pair.second.str(); // This outputs the struct's declaration and methods
                outputFile << "};\n\n";
            }

            // Write function declarations third
            outputFile << functionDeclarations.str();

            // Then write the main function
            outputFile << "int main() {\n" << mainBuffer.str() << "    return 0;\n}";

            outputFile.close();
        }
    }

    std::stringstream& getBufferType() {
        if (insideFunction) {
            return functionDeclarations;
        } else if (insideStructDecl) {
            // Here, use the current struct name to get the correct buffer
            std::cout << "Getting struct strstr: " << m_currentStructName << "\n";
            return structDeclarations[m_currentStructName];
        } else {
            return mainBuffer;
        }
    }

  bool structDeclarationExists(const std::string& structName) {
    // Check if the struct declaration already exists
    return structDeclarations.find(structName) != structDeclarations.end();
  }

    bool isInsideStruct() const {
        return insideStructDecl; 
    }


    std::string convertTurshellType(std::string type);
    void setupNativeFunctions();

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
