#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

std::string read_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        exit(1);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void create_cpp_file(const std::string& embedded_script, const std::string& cpp_file_path) {
    std::string formatted_script = "R\"(" + embedded_script + ")\"";

    std::string cpp_code = 
    "#include <exception>\n"
    "#include <fstream>\n"
    "#include <iostream>\n"
    "#include <string>\n"
    "#include <unordered_set>\n"
    "#include <vector>\n"
    "#include <filesystem>\n"
    "#include \"src/include/AST_Types.h\"\n"
    "#include \"src/include/Lexer.h\"\n"
    "#include \"src/include/Parser.h\"\n"
    "#include \"src/include/TurshellLog.h\"\n"
    "#include \"src/include/Interpretor.h\"\n"
    "#include \"src/include/NativeFunctions.h\"\n"
    "#include \"src/include/TurshellHelp.h\"\n\n"
    "std::string embedded_script = " + formatted_script + ";\n\n"
    "int main() {\n"
    "    try {\n"
    "        // Get the absolute path of the script\n"
    "        std::filesystem::path scriptDir = std::filesystem::current_path();\n\n"
    "        // Preprocess the script for #include directives\n"
    "        std::unordered_set<std::string> includedFiles;\n"
    "        std::string preprocessedScript = preprocessIncludes(embedded_script, scriptDir, includedFiles);\n\n"
    "        // Tokenization\n"
    "        Tokenizer tokenizer(preprocessedScript);\n"
    "        std::vector<Token> tokens;\n"
    "        Token token;\n"
    "        while (token.type != TokenType::EndOfFile) {\n"
    "            token = tokenizer.nextToken();\n"
    "            tokens.push_back(token);\n"
    "        }\n\n"
    "        // Parsing\n"
    "        Parser parser(tokens);\n"
    "        ASTNode* ast = parser.parse();\n\n"
    "        // Interpretation\n"
    "        Interpreter interpreter;\n"
    "        interpreter.setScriptDir(scriptDir);\n"
    "        registerNativeFunctions(&interpreter);\n"
    "        ast->accept(interpreter);\n\n"
    "    } catch (const std::exception& e) {\n"
    "        std::cerr << \"(turshell error) \" << e.what() << std::endl;\n"
    "        return 1;\n"
    "    }\n\n"
    "    return 0;\n"
    "}\n";

    std::ofstream out_file(cpp_file_path);
    if (!out_file) {
        std::cerr << "Error creating file: " << cpp_file_path << std::endl;
        exit(1);
    }
    out_file << cpp_code;
}

void compile_cpp(const std::string& cpp_file_path) {
    std::string compile_command = "g++ -std=c++17 -Os -s -ffunction-sections -fdata-sections -march=native -Wl,--gc-sections  -o compiled_program -Isrc/include src/AST_Types.cpp src/Interpretor.cpp src/Lexer.cpp src/NativeFunctions.cpp src/TurshellLog.cpp src/Visitors.cpp " + cpp_file_path;
    int result = std::system(compile_command.c_str());
    if (result != 0) {
        std::cerr << "Compilation failed." << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: <executable> <path_to_script_file>" << std::endl;
        return 1;
    }

    std::string script_file = argv[1];
    std::string embedded_script = read_file(script_file);

    fs::path script_dir = fs::current_path();
    fs::path cpp_file_path = script_dir / "script.cpp";

    create_cpp_file(embedded_script, cpp_file_path.string());
    compile_cpp(cpp_file_path.string());

    // To run the compiled program, you can use std::system or a similar function.
    // std::system("./compiled_program");

    return 0;
}
