#include "include/TurshellHelp.h"

namespace fs = std::filesystem;


std::string extractInnerTypeFromArrayType(const std::string& arrayType) {
    size_t start = arrayType.find('<');
    size_t end = start;

    if (start == std::string::npos) {
        throw std::runtime_error("Invalid array type format: " + arrayType);
    }

    int bracketCount = 1;
    // Iterate through the string to find the matching closing bracket
    while (bracketCount > 0 && ++end < arrayType.size()) {
        if (arrayType[end] == '<') {
            bracketCount++;
        } else if (arrayType[end] == '>') {
            bracketCount--;
        }
    }

    if (bracketCount != 0 || end == start + 1) {
        throw std::runtime_error("Invalid array type format: " + arrayType);
    }

    return arrayType.substr(start + 1, end - start - 1);
}



bool startsWith(const std::string& fullString, const std::string& starting) {
    // Check if the last occurrence is at the start of the string
    return fullString.rfind(starting, 0) == 0;
}



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

