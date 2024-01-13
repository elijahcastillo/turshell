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


namespace fs = std::filesystem;


std::string extractIncludeFileName(const std::string& includeLine) {
    // Extract the filename from the include directive line
    // Assuming the format is: #include "filename"
    size_t start = includeLine.find("\"") + 1;
    size_t end = includeLine.find("\"", start);
    return includeLine.substr(start, end - start);
}



fs::path getAbsolutePath() {
    // Retrieve the current working directory
    fs::path cwd = fs::current_path();

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

std::string preprocessIncludes(const std::string& scriptContent, const fs::path& baseDir, std::unordered_set<std::string>& includedFiles) {
    std::istringstream stream(scriptContent);
    std::ostringstream processedScript;
    std::string line;

    while (std::getline(stream, line)) {
        if (line.find("#include") == 0) {
            std::string includeFileName = extractIncludeFileName(line); // Implement this function to extract the filename
            fs::path includeFilePath = baseDir / includeFileName;

            if (includedFiles.count(includeFilePath.string()) == 0) {
                includedFiles.insert(includeFilePath.string());
                std::string fileContent = readFileToString(includeFilePath.string());
                processedScript << preprocessIncludes(fileContent, includeFilePath.parent_path(), includedFiles);
            }

        } else {
            processedScript << line << '\n';
        }
    }
    return processedScript.str();
}


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

// Register 'print' - Prints arguments separated by space, ending with a newline
interpreter.registerNativeFunction("print", nativePrint);

// Register 'addr' - Prints out memory addresses of variables
interpreter.registerNativeFunction("addr", nativePrintAddr);

// Register 'copy' - Performs a deep copy of the passed argument
interpreter.registerNativeFunction("copy", nativeCopy);

// Register 'init' - Initializes an array of a specified size with an initializer
interpreter.registerNativeFunction("init", nativeInit);

// Register 'type' - Prints out the types of passed variables
interpreter.registerNativeFunction("type", nativeType);

// Register 'append' - Appends an element or another array to an array
interpreter.registerNativeFunction("append", nativeAppend);

// Register 'input' - Takes a string prompt and returns user input
interpreter.registerNativeFunction("input", nativeInput);

// Register 'len' - Returns the length of an array or a string
interpreter.registerNativeFunction("len", nativeLen);

// Register 'abs' - Returns the absolute value of an integer
interpreter.registerNativeFunction("abs", nativeMathAbs);

// Register 'random' - Generates a random integer
interpreter.registerNativeFunction("random", nativeRandom);

// Register 'pow' - Calculates the power of a number
interpreter.registerNativeFunction("pow", nativeMathPow);

// Register 'sqrt' - Calculates the square root of a number
interpreter.registerNativeFunction("sqrt", nativeMathSqrt);

// Register 'timeNow' - Returns the current time in milliseconds since the epoch
interpreter.registerNativeFunction("timeNow", nativeTimeNow);

// Register 'sleep' - Pauses execution for a specified number of milliseconds
interpreter.registerNativeFunction("sleep", nativeSleep);

// Register 'randRange' - Generates a random integer within a specified range
interpreter.registerNativeFunction("randRange", nativeMathRandRange);

// Register 'toString' - Converts any type to its string representation
interpreter.registerNativeFunction("toString", nativeToString);

// Register 'regexMatch' - Matches a string against a regular expression
interpreter.registerNativeFunction("regexMatch", nativeRegexMatch);

// Register 'assert' - Asserts a condition and optionally throws an error with a message
interpreter.registerNativeFunction("assert", nativeAssert);

// Register 'split' - Splits a string by a delimiter into an array of strings
interpreter.registerNativeFunction("split", nativeStringSplit);

// Register 'hash' - Returns a hash value of the string representation of an argument
interpreter.registerNativeFunction("hash", nativeHash);

// --- New File and Directory Manipulation Functions ---

// Register 'readFile' - Reads and returns the content of a file
interpreter.registerNativeFunction("readFile", nativeFileRead);

// Register 'writeFile' - Writes data to a file
interpreter.registerNativeFunction("writeFile", nativeFileWrite);

// Register 'appendFile' - Appends data to an existing file
interpreter.registerNativeFunction("appendFile", nativeFileAppend);

// Register 'fileExists' - Checks if a file exists
interpreter.registerNativeFunction("fileExists", nativeFileExists);

// Register 'listDirectory' - Lists files and directories in a specified directory
interpreter.registerNativeFunction("listDirectory", nativeDirectoryList);


        ast->accept(interpreter); // Execute the AST
        /* interpreter.printStack(); */



    } catch (const std::exception& e) {
        std::cerr << "(turshell error) " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
