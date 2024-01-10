#include <exception>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
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


namespace fs = std::filesystem;

fs::path getAbsolutePath() {
    // Retrieve the current working directory
    fs::path cwd = fs::current_path();

    // Combine the CWD with the relative path to get the absolute path
    /* fs::path absolutePath = cwd / relativePath; */

    // Convert to absolute path
    return fs::absolute(cwd);
}

std::vector<char> readFileToVector(const std::string& filepath) {
    // Open file in binary mode
    std::ifstream file(filepath, std::ios::binary);

    // Check if the file was opened successfully
    if (!file) {
        throw std::runtime_error("Unable to open file: " + filepath);
    }

    // Determine the file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate space in vector
    std::vector<char> buffer(fileSize);

    // Read the file
    if (fileSize > 0) {
        file.read(buffer.data(), fileSize);
    }

    return buffer;
}

std::string readFileToString(const std::string& filepath) {
    // Open file in binary mode
    std::ifstream file(filepath, std::ios::binary);

    // Check if the file was opened successfully
    if (!file) {
        throw std::runtime_error("Unable to open file: " + filepath);
    }

    // Determine the file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the file into a string
    std::string str;
    if (fileSize > 0) {
        str.resize(fileSize);
        file.read(&str[0], fileSize);
    }

    return str;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: [program] [input_file]\n";
        return 1;
    }

    try {

    // Get the absolute path of the script
    fs::path scriptDir = getAbsolutePath();


    std::cout << "Absolute path of the script: " << scriptDir << std::endl;







        std::string filepath = argv[1];
        std::string buffer = readFileToString(filepath);

        // Tokenization
        Tokenizer tokenizer(buffer);
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
        /* PrintVisitor printer; */
        /* ast->accept(printer); */
        /* std::cout << "========= End of Parser =========\n\n"; */


        //Loger for Interpretor
        TurshellLog::setSettings(TurshellLog::LOG_MEMORY_INFO);

        // Interpretation
        Interpreter interpreter;
        interpreter.setScriptDir(scriptDir);

        //Variable number of arguments of any type, printed seperated by space with a newline at end
        interpreter.registerNativeFunction("print", nativePrint);

        //Prints out address of variable
        interpreter.registerNativeFunction("addr", nativePrintAddr);

        // Preforms a deep copy of whatever passed in
        interpreter.registerNativeFunction("copy", nativeCopy);

        //Print out type of variables passed in seperated by space with a new line at end
        interpreter.registerNativeFunction("type", nativeType);

        //Takes in 2 arguments, first is the array type, second is the value to append
        interpreter.registerNativeFunction("append", nativeAppend);

        //Takes 1 argument of type string, returns the value gotten from IO as string or int if possible
        interpreter.registerNativeFunction("input", nativeInput);

        //Take 1 argument array, returns the # of items in the array as an int
        interpreter.registerNativeFunction("len", nativeLen);

        interpreter.registerNativeFunction("readFile", nativeFileRead);

        interpreter.registerNativeFunction("abs", nativeMathAbs);
        interpreter.registerNativeFunction("random", nativeRandom);
        interpreter.registerNativeFunction("pow", nativeMathPow);
        interpreter.registerNativeFunction("sqrt", nativeMathSqrt);

        //No arguments, returns a float for the miliseconds since epoch
        interpreter.registerNativeFunction("timeNow", nativeTimeNow);

        //1 argument of type int, number of milisecodns to stop execution
        interpreter.registerNativeFunction("sleep", nativeSleep);

        //Random int between the range
        interpreter.registerNativeFunction("randRange", nativeMathRandRange);

        //Takes 1 argument of any time and creates the string represenation of it; 
        interpreter.registerNativeFunction("toString", nativeToString);

        //Takes 2 arguments, 1st is a string and 2nd is the regex to match the string, returns bool
        interpreter.registerNativeFunction("regexMatch", nativeRegexMatch);

        //Takes 1 or 2 argumetns, first is bool, seconds is error message is false
        interpreter.registerNativeFunction("assert", nativeAssert);

        //Takes 2 arguments of type stiing, 1st is the string to seach, second is the dilimeter, returns array of string
        interpreter.registerNativeFunction("split", nativeStringSplit);
        
        //Takes in 1 argument of any type and reutrns an int of the hast of its toString
        interpreter.registerNativeFunction("hash", nativeHash);

        ast->accept(interpreter); // Execute the AST
        /* interpreter.printStack(); */



    } catch (const std::exception& e) {
        std::cerr << "(turshell error) " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
