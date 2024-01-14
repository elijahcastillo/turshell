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



int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: [program] [input_file]\n";
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




        // Interpretation
        Interpreter interpreter;
        interpreter.setScriptDir(scriptDir);


        // Register the native functions in your scripting language
        registerNativeFunctions(&interpreter);



        ast->accept(interpreter); // Execute the AST
        /* interpreter.printStack(); */



    } catch (const std::exception& e) {
        std::cerr << "(turshell error) " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
