#pragma once
#include "Runtime.h"
#include "Interpretor.h"
#include <iostream>
#include <memory>
#include <stdexcept>

// Example of a native function
std::shared_ptr<RuntimeVal> nativePrint(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {
    for (const auto& arg : args) {
        // Assuming a method in RuntimeVal to get its string representation
        std::cout << arg->toString() << " ";
    }

    std::cout << std::endl;
    return std::make_shared<StringValue>("E"); // Return a dummy value or void
}


std::shared_ptr<RuntimeVal> input(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args) {

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
