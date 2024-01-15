#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <unordered_set>
#include <vector>
#include <filesystem> // For path manipulation and retrieval

namespace fs = std::filesystem;


std::string extractInnerTypeFromArrayType(const std::string& arrayType);



bool startsWith(const std::string& fullString, const std::string& starting);



std::string extractIncludeFileName(const std::string& includeLine); 



fs::path getAbsolutePath();

std::vector<char> readFileToVector(const std::string& filepath);

std::string readFileToString(const std::string& filepath);

std::string preprocessIncludes(const std::string& scriptContent, const fs::path& baseDir, std::unordered_set<std::string>& includedFiles);

