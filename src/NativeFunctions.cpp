#include "include/NativeFunctions.h"
#include "include/Runtime.h"
#include <random>
#include <cmath>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <regex>





// Example hash function
unsigned int simpleHash(const std::string& str) {
    unsigned int hash = 0;
    for (char ch : str) {
        hash = hash * 31 + static_cast<unsigned char>(ch);
    }
    return hash;
}

std::shared_ptr<RuntimeVal> nativeHash(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 1) {
        throw std::runtime_error("nativeHash expects one argument");
    }

    std::string str = args[0]->toString();

    unsigned int hashValue = simpleHash(str);
    return std::make_shared<IntValue>(hashValue);
}


// Example of a native function
std::shared_ptr<RuntimeVal> nativePrint(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    for (const auto& arg : args) {
        // Assuming a method in RuntimeVal to get its string representation
        std::cout << arg->toString() << " ";
    }

    std::cout << std::endl;
    return std::make_shared<BoolValue>(true);
}


std::shared_ptr<RuntimeVal> nativeType(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args){
    for (const auto& arg : args) {
        // Assuming a method in RuntimeVal to get its string representation
        std::cout << arg->getType() << " ";
    }

    std::cout << std::endl;
    return std::make_shared<BoolValue>(true);
}


std::shared_ptr<RuntimeVal> nativeStringSplit(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 2 || args[0]->getType() != "string" || args[1]->getType() != "string") {
        throw std::runtime_error("nativeStringSplit expects two string arguments");
    }

    std::string str = static_cast<StringValue*>(args[0].get())->getValue();
    std::string delimiter = static_cast<StringValue*>(args[1].get())->getValue();

    std::vector<std::shared_ptr<RuntimeVal>> splitArray;
    size_t start = 0;
    size_t end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        splitArray.push_back(std::make_shared<StringValue>(str.substr(start, end - start)));
        start = end + delimiter.length();
    }

    // Add the last part
    splitArray.push_back(std::make_shared<StringValue>(str.substr(start)));

    return std::make_shared<ArrayValue>("string", splitArray);  // Assuming ArrayValue is an appropriate type
}


std::shared_ptr<RuntimeVal> nativeAssert(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() < 1 || args.size() > 2) {
        throw std::runtime_error("nativeAssert expects 1 or 2 arguments");
    }

    bool condition = static_cast<BoolValue*>(args[0].get())->getValue();
    std::string message = "Assertion failed";

    if (args.size() == 2 && args[1]->getType() == "string") {
        message = static_cast<StringValue*>(args[1].get())->getValue();
    }

    if (!condition) {
        throw std::runtime_error(message);
    }

    return std::make_shared<BoolValue>(true);
}


// Type Conversion: Convert to string
std::shared_ptr<RuntimeVal> nativeToString(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 1) {
        throw std::runtime_error("nativeToString expects one argument");
    }
    return std::make_shared<StringValue>(args[0]->toString());
}

// Regular Expressions: Match a string against a regex
std::shared_ptr<RuntimeVal> nativeRegexMatch(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 2 || args[0]->getType() != "string" || args[1]->getType() != "string") {
        throw std::runtime_error("nativeRegexMatch expects two string arguments");
    }
    std::string str = static_cast<StringValue*>(args[0].get())->getValue();
    std::string regexPattern = static_cast<StringValue*>(args[1].get())->getValue();
    std::regex pattern(regexPattern);
    bool match = std::regex_match(str, pattern);
    return std::make_shared<BoolValue>(match);
}




// Timing: Get current time in milliseconds since the epoch
std::shared_ptr<RuntimeVal> nativeTimeNow(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return std::make_shared<FloatValue>(static_cast<int>(millis));
}

// Timing: Sleep for a specified number of milliseconds
std::shared_ptr<RuntimeVal> nativeSleep(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 1 || args[0]->getType() != "int") {
        throw std::runtime_error("nativeSleep expects one integer argument");
    }
    int millis = static_cast<IntValue*>(args[0].get())->getValue();
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    return std::make_shared<BoolValue>(true); // Return a dummy value or void
}

// Math: Generate a random number within a specified range
std::shared_ptr<RuntimeVal> nativeMathRandRange(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 2 || args[0]->getType() != "int" || args[1]->getType() != "int") {
        throw std::runtime_error("nativeMathRandRange expects two integer arguments");
    }
    int min = static_cast<IntValue*>(args[0].get())->getValue();
    int max = static_cast<IntValue*>(args[1].get())->getValue();

    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(min, max);

    return std::make_shared<IntValue>(distribution(generator));
}


// String Manipulation: Convert to uppercase
std::shared_ptr<RuntimeVal> nativeStringUpper(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 1 || args[0]->getType() != "string") {
        throw std::runtime_error("nativeStringUpper expects one string argument");
    }
    std::string str = static_cast<StringValue*>(args[0].get())->getValue();
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return std::make_shared<StringValue>(str);
}





std::shared_ptr<RuntimeVal> nativeInput(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {

    // Ensure there is exactly one argument
    if (args.size() != 1) {
        throw std::runtime_error("Native function 'input' expects exactly 1 argument");
    }

    // Ensure the argument is a string
    if (args[0]->getType() != "string") {

        throw std::runtime_error("Native function 'input' expects a string argument");
    }

    auto inputPrint = static_cast<StringValue*>(args[0].get());

    std::string value;
    std::cout << inputPrint->getValue();
    std::getline(std::cin, value);


    // Try to convert to number if possible
    try {
        size_t pos;
        int numValue = std::stoi(value, &pos);
        // Check if the entire string was converted to an integer
        if (pos == value.length()) {
            return std::make_shared<IntValue>(numValue);
        }
    } catch (const std::invalid_argument& e) {
        // If conversion fails, return the string
        return std::make_shared<StringValue>(value);
    } catch (const std::out_of_range& e) {
        // Handle out-of-range numbers
    }

    // Return as string if it's not a valid integer
    return std::make_shared<StringValue>(value);
}





std::shared_ptr<RuntimeVal> nativeAppend(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    // Ensure there is exactly one argument
    if (args.size() != 2) {
        throw std::runtime_error("Native function 'input' expects exactly 2 argument Ex: append(arr, 2)");
    }

    auto arrayRuntimeValue = dynamic_cast<ArrayValue*>(args[0].get());
    if(arrayRuntimeValue == nullptr){
        throw std::runtime_error("Native function 'append' expects an array as first argument");
    }

    std::shared_ptr<RuntimeVal> value = args[1];

   
    //Handle concat arrays  Ex: append(arr, [1,2,3])
    auto valueArray = dynamic_cast<ArrayValue*>(args[1].get());
    if(valueArray != nullptr){
     
      if(arrayRuntimeValue->getType() != valueArray->getType()){
              throw std::runtime_error("Native function 'append': non matching types, array of " + arrayRuntimeValue->getType() + " != " + valueArray->getType());
      }

      //Concat arrays
      for(auto value: valueArray->elements){
        arrayRuntimeValue->addElement(value);
      }

      return std::make_shared<BoolValue>(true); // Return a dummy value or void
    }

    //Handle single elements
    if(arrayRuntimeValue->elementType != value->getType()){

        throw std::runtime_error("Native function 'append': non matching types, array of " + arrayRuntimeValue->elementType + " != " + value->getType());
    }

    arrayRuntimeValue->addElement(value);

    
    return std::make_shared<BoolValue>(true); // Return a dummy value or void
}






std::shared_ptr<RuntimeVal> nativeLen(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    // Ensure there is exactly one argument
    if (args.size() != 1) {
        throw std::runtime_error("nativeLen expects exactly one argument");
    }

    std::shared_ptr<RuntimeVal> arg = args[0];

    // Handle string type
    if (arg->getType() == "string") {
        std::string strValue = static_cast<StringValue*>(arg.get())->getValue();
        return std::make_shared<IntValue>(strValue.length());
    }
    
    // Handle array type
    else if (arg->getType().rfind("array<", 0) == 0) { // Checks if type starts with "array<"
        ArrayValue* arrayValue = dynamic_cast<ArrayValue*>(arg.get());
        if (arrayValue == nullptr) {
            throw std::runtime_error("nativeLen: Argument is not a valid array");
        }
        return std::make_shared<IntValue>(arrayValue->elements.size());
    }

    // If argument is neither a string nor an array
    throw std::runtime_error("nativeLen expects a string or an array argument");
}


std::shared_ptr<RuntimeVal> nativeMathAbs(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 1 || args[0]->getType() != "int") {
        throw std::runtime_error("nativeMathAbs expects exactly one integer argument");
    }
    int number = static_cast<IntValue*>(args[0].get())->getValue();
    return std::make_shared<IntValue>(std::abs(number));
}



std::shared_ptr<RuntimeVal> nativeRandom(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    static std::mt19937 generator(std::random_device{}());

    std::uniform_int_distribution<int> distribution(0, INT_MAX);
    return std::make_shared<IntValue>(distribution(generator));
}


std::shared_ptr<RuntimeVal> nativeMathPow(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 2 || args[0]->getType() != "int" || args[1]->getType() != "int") {
        throw std::runtime_error("nativeMathPow expects two integer arguments");
    }

    int base = static_cast<IntValue*>(args[0].get())->getValue();
    int exponent = static_cast<IntValue*>(args[1].get())->getValue();

    return std::make_shared<IntValue>(static_cast<int>(std::pow(base, exponent)));
}


std::shared_ptr<RuntimeVal> nativeMathSqrt(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 1 || args[0]->getType() != "int") {
        throw std::runtime_error("nativeMathSqrt expects one integer argument");
    }

    int number = static_cast<IntValue*>(args[0].get())->getValue();
    if (number < 0) {
        throw std::runtime_error("nativeMathSqrt: square root of a negative number is not defined");
    }

    return std::make_shared<IntValue>(static_cast<int>(std::sqrt(number)));
}


std::shared_ptr<RuntimeVal> nativeFileRead(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    if (args.size() != 1 || args[0]->getType() != "string") {
        throw std::runtime_error("nativeFileRead expects one string argument for the file path");
    }

    std::string filePath = static_cast<StringValue*>(args[0].get())->getValue();
    // Resolve filePath based on the context of the calling file

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("nativeFileRead: unable to open file " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return std::make_shared<StringValue>(buffer.str());
}
