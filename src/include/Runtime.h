#pragma once
#include <variant>
#include <string>
#include <stdexcept>

struct RuntimeVal {
    // Define a variant type that can hold an int, bool, or string
    std::variant<int, bool, std::string> value;

    // Constructors for different types
    RuntimeVal(int intValue) : value(intValue) {}
    RuntimeVal(bool boolValue) : value(boolValue) {}
    RuntimeVal(const std::string& stringValue) : value(stringValue) {}
    RuntimeVal() {}

    // Helper methods to get the value (you can add more as needed)
    int getInt() const {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        }
        throw std::runtime_error("RuntimeVal does not hold an int");
    }

    bool getBool() const {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
        throw std::runtime_error("RuntimeVal does not hold a bool");
    }

    std::string getString() const {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        throw std::runtime_error("RuntimeVal does not hold a string");
    }
};
