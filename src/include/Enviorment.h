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
    std::unordered_map<std::string, RuntimeVal> variables;
    std::shared_ptr<Environment> parent;

    Environment(std::shared_ptr<Environment> parent = nullptr) : parent(parent) {}

    void setVariable(const std::string& name, RuntimeVal value, VariableSettings setting) {


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
