#pragma once
#include <variant>
#include <string>
#include <stdexcept>
#include <memory>
#include <map>
#include <vector>
#include <iostream>





struct RuntimeVal {
  std::string type;

  RuntimeVal(std::string typeStr): type(typeStr){};
  virtual ~RuntimeVal() {
    /* std::cout << "Base RuntimeVal of type " << type << " is being destroyed\n"; */
  }
  virtual std::string toString() = 0;
virtual std::shared_ptr<RuntimeVal> copy() const = 0;
    virtual std::shared_ptr<RuntimeVal> getProperty(const std::string& name) {
        throw std::runtime_error("Not a struct type");
    }

    virtual void setProperty(const std::string& name, std::shared_ptr<RuntimeVal> value) {
        throw std::runtime_error("Not a struct type");
    }
  std::string getType() { return type; };
};

struct IntValue : public RuntimeVal{
  int value;

  IntValue(int value): RuntimeVal("int"), value(value) {};
    ~IntValue() override {
        /* std::cout << "IntValue destructor called with value of " << value <<  "\n"; */
    }

  int getValue() { return value; };

  std::shared_ptr<RuntimeVal> copy() const override {
    return std::make_shared<IntValue>(value);
  }

  std::string toString() override {
    return std::to_string(value);
  }
};

struct FloatValue : public RuntimeVal{
  float value;

  FloatValue(float value): RuntimeVal("float"), value(value) {};

  float getValue() { return value; };

  std::shared_ptr<RuntimeVal> copy() const override {
    return std::make_shared<FloatValue>(value);
  }

  std::string toString() override {
    return std::to_string(value);
  }
};


struct StringValue : public RuntimeVal {
    std::string value;

    StringValue(const std::string& value) : RuntimeVal("string"), value(value) {};
    std::string toString() override { return value; };
    std::string getValue() { return value; };

  std::shared_ptr<RuntimeVal> copy() const override {
    return std::make_shared<StringValue>(value);
  }
};


struct BoolValue : public RuntimeVal {
    bool value;

    BoolValue(bool value) : RuntimeVal("bool"), value(value) {};
    std::string toString() override { return value ? "true" : "false"; };
    bool getValue() { return value; };

    std::shared_ptr<RuntimeVal> copy() const override {
      return std::make_shared<BoolValue>(value);
    }
};


struct StructValue : public RuntimeVal {
    std::map<std::string, std::shared_ptr<RuntimeVal>> properties;
    std::string structName;

    StructValue(std::string structName) : RuntimeVal(structName), structName(structName) {}

    void setStructType(const std::string& type) {
        this->type = type; // Set the RuntimeVal type to the struct type
        this->structName = type; // Optionally set structName if needed
    }

    std::shared_ptr<RuntimeVal> getProperty(const std::string& name) override {
        auto it = properties.find(name);
        if (it != properties.end()) {
            return it->second;
        }
        throw std::runtime_error("Property not found: " + name);
    }

    void setProperty(const std::string& name, std::shared_ptr<RuntimeVal> value) override {
        properties[name] = value;
    }

  std::shared_ptr<RuntimeVal> copy() const override {
    auto cloned = std::make_shared<StructValue>(structName);
    for (const auto& pair : properties) {
      cloned->properties[pair.first] = pair.second->copy();
    }
    return cloned;
  }

std::string toString() override {
    std::string result = "Struct{";
    bool first = true;
    for (const auto& pair : properties) {
        if (!first) {
            result += ", ";
        }
        first = false;
        result += pair.first + ": ";
        if (pair.second) {
            result += pair.second->toString();
        } else {
            result += "null";
        }
    }
    result += "}";
    return result;
}

    // toString implementation...
};



struct ArrayValue : public RuntimeVal {
    std::vector<std::shared_ptr<RuntimeVal>> elements;
    std::string elementType;

    ArrayValue(std::string elementType, std::vector<std::shared_ptr<RuntimeVal>>& elements)
        : RuntimeVal("array<" + elementType + ">"), elementType(elementType), elements(elements) {
        }

    void addElement(std::shared_ptr<RuntimeVal> element) {
        // Optional: Check if the element's type matches elementType
        elements.push_back(element);
    }

    void setType(std::string type){
      type = "array<" + type + ">";
      elementType = type;
    }

    std::shared_ptr<RuntimeVal> getElement(int index) {
        if (index < 0 || index >= elements.size()) {
            throw std::runtime_error("Array index out of bounds");
        }
        return elements[index];
    }

    void setElement(int index, std::shared_ptr<RuntimeVal> element) {
        /* std::cout << "Array setElement at index '" << index << "' at addr " << element << " of value " << element->toString() << "\n"; */



        // Optional: Check if the element's type matches elementType
        if (index < 0 || index >= elements.size()) {
            throw std::runtime_error("Array index out of bounds");
        }
        elements[index] = element;
    }


  std::shared_ptr<RuntimeVal> copy() const override {
    std::vector<std::shared_ptr<RuntimeVal>> clonedElements;
    for (const auto& element : elements) {
      clonedElements.push_back(element->copy());
    }
    return std::make_shared<ArrayValue>(elementType, clonedElements);
  }

    std::string toString() override {
        std::string result = "Array[";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) {
                result += ", ";
            }
            result += elements[i]->toString();
        }
        result += "]";
        return result;
    }

    // Additional methods as needed...
};





class RuntimeOps {
public:
    static std::shared_ptr<RuntimeVal> add(const std::shared_ptr<RuntimeVal>& left, const std::shared_ptr<RuntimeVal>& right) {
        if (left->getType() == "int" && right->getType() == "int") {
            return std::make_shared<IntValue>(static_cast<IntValue*>(left.get())->getValue() +
                                              static_cast<IntValue*>(right.get())->getValue());
        } else if (left->getType() == "float" && right->getType() == "float") {
            return std::make_shared<FloatValue>(static_cast<FloatValue*>(left.get())->getValue() +
                                                static_cast<FloatValue*>(right.get())->getValue());
        } else if (left->getType() == "string" && right->getType() == "string") {
            return std::make_shared<StringValue>(static_cast<StringValue*>(left.get())->getValue() +
                                                 static_cast<StringValue*>(right.get())->getValue());
        }
        throw std::runtime_error("Unsupported types for addition");
    }

    static std::shared_ptr<RuntimeVal> subtract(const std::shared_ptr<RuntimeVal>& left, const std::shared_ptr<RuntimeVal>& right) {
        if (left->getType() == "int" && right->getType() == "int") {
            return std::make_shared<IntValue>(static_cast<IntValue*>(left.get())->getValue() -
                                              static_cast<IntValue*>(right.get())->getValue());
        } else if (left->getType() == "float" && right->getType() == "float") {
            return std::make_shared<FloatValue>(static_cast<FloatValue*>(left.get())->getValue() -
                                                static_cast<FloatValue*>(right.get())->getValue());
        } 
        throw std::runtime_error("Unsupported types for subtraction");
    }


    static std::shared_ptr<RuntimeVal> multiply(const std::shared_ptr<RuntimeVal>& left, const std::shared_ptr<RuntimeVal>& right) {
        if (left->getType() == "int" && right->getType() == "int") {
            return std::make_shared<IntValue>(static_cast<IntValue*>(left.get())->getValue() *
                                              static_cast<IntValue*>(right.get())->getValue());
        } else if (left->getType() == "float" && right->getType() == "float") {
            return std::make_shared<FloatValue>(static_cast<FloatValue*>(left.get())->getValue() *
                                                static_cast<FloatValue*>(right.get())->getValue());
        } 
        throw std::runtime_error("Unsupported types for multiplication");
    }


    static std::shared_ptr<RuntimeVal> divide(const std::shared_ptr<RuntimeVal>& left, const std::shared_ptr<RuntimeVal>& right) {
        if (left->getType() == "int" && right->getType() == "int") {
            return std::make_shared<IntValue>(static_cast<IntValue*>(left.get())->getValue() /
                                              static_cast<IntValue*>(right.get())->getValue());
        } else if (left->getType() == "float" && right->getType() == "float") {
            return std::make_shared<FloatValue>(static_cast<FloatValue*>(left.get())->getValue() /
                                                static_cast<FloatValue*>(right.get())->getValue());
        } 
        throw std::runtime_error("Unsupported types for division");
    }


    static std::shared_ptr<RuntimeVal> mod(const std::shared_ptr<RuntimeVal>& left, const std::shared_ptr<RuntimeVal>& right) {
        if (left->getType() == "int" && right->getType() == "int") {
            return std::make_shared<IntValue>(static_cast<IntValue*>(left.get())->getValue() %
                                              static_cast<IntValue*>(right.get())->getValue());
        } 
        throw std::runtime_error("Unsupported types for mod");
    }



    static std::shared_ptr<RuntimeVal> compare(const std::shared_ptr<RuntimeVal>& left, const std::shared_ptr<RuntimeVal>& right, const std::string& op) {


        if (left->getType() == "float" && right->getType() == "float") {
            float leftValue = static_cast<FloatValue*>(left.get())->getValue();
            float rightValue = static_cast<FloatValue*>(right.get())->getValue();
            if (op == "==") return std::make_shared<BoolValue>(leftValue == rightValue);
            if (op == "!=") return std::make_shared<BoolValue>(leftValue != rightValue);
            if (op == ">") return std::make_shared<BoolValue>(leftValue > rightValue);
            if (op == "<") return std::make_shared<BoolValue>(leftValue <  rightValue);
            if (op == ">=") return std::make_shared<BoolValue>(leftValue >= rightValue);
            if (op == "<=") return std::make_shared<BoolValue>(leftValue <= rightValue);
            // Add cases for other comparison operators
        } 
        if (left->getType() == "int" && right->getType() == "int") {
            int leftValue = static_cast<IntValue*>(left.get())->getValue();
            int rightValue = static_cast<IntValue*>(right.get())->getValue();
            if (op == "==") return std::make_shared<BoolValue>(leftValue == rightValue);
            if (op == "!=") return std::make_shared<BoolValue>(leftValue != rightValue);
            if (op == ">") return std::make_shared<BoolValue>(leftValue > rightValue);
            if (op == "<") return std::make_shared<BoolValue>(leftValue <  rightValue);
            if (op == ">=") return std::make_shared<BoolValue>(leftValue >= rightValue);
            if (op == "<=") return std::make_shared<BoolValue>(leftValue <= rightValue);
            // Add cases for other comparison operators
        } 

        if (left->getType() == "string" && right->getType() == "string") {
          std::string leftValue = static_cast<StringValue*>(left.get())->getValue();
          std::string rightValue = static_cast<StringValue*>(right.get())->getValue();
            if (op == "==") return std::make_shared<BoolValue>(leftValue == rightValue);
            if (op == "!=") return std::make_shared<BoolValue>(leftValue != rightValue);
        }


        if (left->getType() == "bool" && right->getType() == "bool") {
            bool leftValue = static_cast<BoolValue*>(left.get())->getValue();
            bool rightValue = static_cast<BoolValue*>(right.get())->getValue();
            if (op == "==") return std::make_shared<BoolValue>(leftValue == rightValue);
            if (op == "!=") return std::make_shared<BoolValue>(leftValue == rightValue);
        }



        // Add cases for string comparison and others if needed
        throw std::runtime_error("Unsupported types for comparison");
    }



};
