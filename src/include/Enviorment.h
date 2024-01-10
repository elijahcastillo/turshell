#pragma once
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include "Runtime.h"
#include "TurshellLog.h"

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

     TurshellLog::Log("Setting variable '" + name + "' to addr " + pointerAddrToString(value.get()) + " of value " + value->toString() + " with setting " + std::to_string(setting) , TurshellLog::LOG_MEMORY_INFO);


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
            /* std::cout << "Geting varible in parent" << std::endl; */
            return parent->getVariable(name);
        } else {
            throw std::runtime_error("Variable not defined: " + name);
        }


    }

    void printCurrent(){
      std::cout << "Inside Env current env\n";
      for (const auto& pair : variables) {
            std::cout << "Variable: " << pair.first << ", Value: " << pair.second->toString() << std::endl;
        }
    }

    void printParent(){
      std::cout << "Inside Env PARENT env\n";
      if(parent){
      for (const auto& pair : parent->variables) {
            std::cout << "Variable: " << pair.first << ", Value: " << pair.second->toString() << std::endl;
        }
      }

    }

    // Add methods for scope management if needed
};
