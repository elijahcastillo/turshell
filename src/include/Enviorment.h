#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include "Runtime.h"

class Environment {
    std::unordered_map<std::string, RuntimeVal> variables;
    std::shared_ptr<Environment> parent;

public:
    Environment(std::shared_ptr<Environment> parent = nullptr) : parent(parent) {}

    void setVariable(const std::string& name, RuntimeVal value) {
        variables[name] = value;
    }

    RuntimeVal getVariable(const std::string& name) {
        if (variables.find(name) != variables.end()) {
            return variables[name];
        } else if (parent != nullptr) {
            return parent->getVariable(name);
        } else {
            throw std::runtime_error("Variable not defined: " + name);
        }
    }

    // Add methods for scope management if needed
};
