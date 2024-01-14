import os
import subprocess
import sys
import glob

def read_file(file_path):
    with open(file_path, 'r') as file:
        return file.read()

def create_cpp_file(embedded_script, cpp_file_path):
    cpp_code = f"""
    #include <exception>
    #include <fstream>
    #include <iostream>
    #include <string>
    #include <unordered_set>
    #include <vector>
    #include <filesystem>
    #include "src/include/AST_Types.h"
    #include "src/include/Lexer.h"
    #include "src/include/Parser.h"
    #include "src/include/TurshellLog.h"
    #include "src/include/Interpretor.h"
    #include "src/include/NativeFunctions.h"
    #include "src/include/TurshellHelp.h"

    std::string embedded_script = R\"({embedded_script})\";

    int main() {{
        try {{
            // Get the absolute path of the script
            std::filesystem::path scriptDir = std::filesystem::current_path();

            // Preprocess the script for #include directives
            std::unordered_set<std::string> includedFiles;
            std::string preprocessedScript = preprocessIncludes(embedded_script, scriptDir, includedFiles);

            // Tokenization
            Tokenizer tokenizer(preprocessedScript);
            std::vector<Token> tokens;
            Token token;
            while (token.type != TokenType::EndOfFile) {{
                token = tokenizer.nextToken();
                tokens.push_back(token);
            }}

            // Parsing
            Parser parser(tokens);
            ASTNode* ast = parser.parse();

            // Interpretation
            Interpreter interpreter;
            interpreter.setScriptDir(scriptDir);
            registerNativeFunctions(&interpreter);
            ast->accept(interpreter);

        }} catch (const std::exception& e) {{
            std::cerr << "(turshell error) " << e.what() << std::endl;
            return 1;
        }}

        return 0;
    }}
    """
    with open(cpp_file_path, 'w') as file:
        file.write(cpp_code)

def compile_cpp(cpp_file_path, output_path):
    # List all .cpp files in src directory, excluding main.cpp
    src_cpp_files = glob.glob('src/*.cpp')
    src_cpp_files = [file for file in src_cpp_files if not file.endswith('main.cpp')]

    compile_command = f"g++ -std=c++17 -Os -s -ffunction-sections -fdata-sections -march=native -Wl,--gc-sections -o {output_path} {cpp_file_path} {' '.join(src_cpp_files)} -Isrc/include"

    subprocess.run(compile_command, shell=True, check=True)

def run_cpp(output_path):
    subprocess.run(output_path, check=True)

# Check if a file path argument is provided
if len(sys.argv) != 2:
    print("Usage: python your_script.py <path_to_script_file>")
    sys.exit(1)

# Read the file from the caller's directory
embedded_script = read_file(sys.argv[1])

# Get the directory where the Python script is located
script_dir = os.path.dirname(os.path.abspath(__file__))

cpp_file_path = os.path.join(script_dir, "script.cpp")
output_path = os.path.join(script_dir, "compiled_program")

create_cpp_file(embedded_script, cpp_file_path)
compile_cpp(cpp_file_path, output_path)
# run_cpp(output_path)
