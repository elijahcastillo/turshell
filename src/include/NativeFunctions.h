#pragma once
#include "Runtime.h"
#include "Interpretor.h"
#include <iostream>
#include <stdexcept>

// Example of a native function
RuntimeVal nativePrint(Interpreter& interpreter, std::vector<RuntimeVal>& args) {
    for (const auto& arg : args) {
        // Assuming a method in RuntimeVal to get its string representation
        std::cout << arg.toString() << " ";
    }

    std::cout << std::endl;
    return RuntimeVal(); // Return a dummy value or void
}


RuntimeVal input(Interpreter& interpreter, std::vector<RuntimeVal>& args) {

    // Ensure there is exactly one argument
    if (args.size() != 1) {
        throw std::runtime_error("Native function 'input' expects exactly 1 argument");
    }

    // Ensure the argument is a string
    if (!args[0].isString()) {
        throw std::runtime_error("Native function 'input' expects a string argument");
    }

    std::string value;
    std::cout << args[0].getString();
    std::cin >> value;

    // Try to convert to number if possible
    try {
        size_t pos;
        int numValue = std::stoi(value, &pos);
        // Check if the entire string was converted to an integer
        if (pos == value.length()) {
            return RuntimeVal(numValue);
        }
    } catch (const std::invalid_argument& e) {
        // If conversion fails, return the string
        return RuntimeVal(value);
    } catch (const std::out_of_range& e) {
        // Handle out-of-range numbers
    }

    // Return as string if it's not a valid integer
    return RuntimeVal(value);
}
