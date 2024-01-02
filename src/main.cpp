#include <exception>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include "include/AST_Types.h"
#include "include/Lexer.h"
#include "include/Parser.h"
#include "include/Visitors.h"
#include "include/Interpretor.h"
#include "include/NativeFunctions.h"

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
        std::string filepath = argv[1];
        std::string buffer = readFileToString(filepath);

        // Tokenization
        Tokenizer tokenizer(buffer);
        std::vector<Token> tokens;
        Token token;
        while (token.type != TokenType::EndOfFile) {
            token = tokenizer.nextToken();
            token.print();
            tokens.push_back(token);
            std::cout << "\n";
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
        interpreter.registerNativeFunction("print", nativePrint);
        interpreter.registerNativeFunction("input", input);

        ast->accept(interpreter); // Execute the AST
        interpreter.printStack();



    } catch (const std::exception& e) {
        std::cerr << "(trushell error) " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
