#include <exception>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <unordered_set>
#include <vector>
#include <filesystem> // For path manipulation and retrieval
#include <cstdlib>    // For getenv
#include "include/AST_Types.h"
#include "include/Lexer.h"
#include "include/Parser.h"
#include "include/TurshellLog.h"
#include "include/Visitors.h"
#include "include/Interpretor.h"
#include "include/NativeFunctions.h"
#include "include/TurshellHelp.h"
#include "include/CppTranspiler.h"



int main(int argc, char* argv[]) {
    bool cppTranspilerMode = false;
    std::string cppOutputFilename;

    // Parse command line arguments
    if (argc < 2) {
        std::cerr << "Usage: [program] [input_file] [-c [output_cpp_file]]\n";
        return 1;
    }     

    //Logger
    /* TurshellLog::setSettings(TurshellLog::LOG_INFO); */

    try {

    // Get the absolute path of the script
    fs::path scriptDir = getAbsolutePath();

    TurshellLog::Log("Absolute path of the script: " + scriptDir.generic_string(), TurshellLog::LOG_INFO);


        std::string filepath = argv[1];
        std::string buffer = readFileToString(filepath);


    // Check for the optional C++ transpiler flag
    if (argc > 2) {
        for (int i = 2; i < argc; ++i) {
            if (std::string(argv[i]) == "-c") {
                cppTranspilerMode = true;
                if (i + 1 < argc) {
                    cppOutputFilename = argv[i + 1];
                } else {
                    std::cerr << "Error: Missing output filename for C++ transpiler.\n";
                    return 1;
                }
                break;
            }
        }
    }



        // Preprocess the script for #include directives
        std::unordered_set<std::string> includedFiles;
        std::string preprocessedScript = preprocessIncludes(buffer, scriptDir, includedFiles);


        // Tokenization
        Tokenizer tokenizer(preprocessedScript);
        std::vector<Token> tokens;
        Token token;
        while (token.type != TokenType::EndOfFile) {
            token = tokenizer.nextToken();
            tokens.push_back(token);
            /* token.print(); */
            /* std::cout << "\n"; */
        }

        // Parsing
        Parser parser(tokens);
        ASTNode* ast = parser.parse();

        // Print the AST (for debugging or verification)
        PrintVisitor printer;
        ast->accept(printer);
        std::cout << "========= End of Parser =========\n\n";



        // If C++ transpiler mode is enabled, use the transpiler
        if (cppTranspilerMode) {
            CppTranspilerVisitor cppTranspiler(cppOutputFilename);
            ast->accept(cppTranspiler);
            std::cout << "C++ code has been generated in: " << cppOutputFilename << std::endl;
        } else {
            // Normal interpretation
            Interpreter interpreter;
            interpreter.setScriptDir(scriptDir);
            registerNativeFunctions(&interpreter);
            ast->accept(interpreter);
        }




    } catch (const std::exception& e) {
        std::cerr << "(turshell error) " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
