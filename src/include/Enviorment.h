#pragma once
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include "Runtime.h"

enum VariableSettings{
  Declaration,
  Assignment
};

class Environment {

  public:
    std::unordered_map<std::string, std::shared_ptr<RuntimeVal>> variables;
    std::shared_ptr<Environment> parent;

    Environment(std::shared_ptr<Environment> parent = nullptr) : parent(parent) {}

    void setVariable(const std::string& name, std::shared_ptr<RuntimeVal> value, VariableSettings setting) {
      /* std::cout << "Setting variable '" << name << "' to " << value->toString() << " with setting " << setting << std::endl; */

      if(setting == Declaration){
        variables[name] = value;
        return;
      } 

      if(setting == Assignment){
        

        //In current scope
        if(variables.find(name) != variables.end()){
          variables[name] = value;
        } else {
          //Look in parent
          if(parent != nullptr && (parent->variables.find(name) != parent->variables.end())){

            parent->variables[name] = value;
          } else {

            throw std::runtime_error("Variable not defined: " + name);
          }
        }
      }
      

    }

    std::shared_ptr<RuntimeVal> getVariable(const std::string& name) {
/* std::cout << "Getting variable '" << name << "'" << std::endl; */
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
