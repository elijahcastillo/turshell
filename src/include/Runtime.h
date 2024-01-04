#pragma once
#include <variant>
#include <string>
#include <stdexcept>
#include <memory>
#include <map>





struct RuntimeVal {
  std::string type;

  RuntimeVal(std::string typeStr): type(typeStr){};
  virtual ~RuntimeVal() = default;
  virtual std::string toString() = 0;
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

  int getValue() { return value; };

  std::string toString() override {
    return std::to_string(value);
  }
};


struct StringValue : public RuntimeVal {
    std::string value;

    StringValue(const std::string& value) : RuntimeVal("string"), value(value) {};
    std::string toString() override { return value; };
    std::string getValue() { return value; };
};


struct BoolValue : public RuntimeVal {
    bool value;

    BoolValue(bool value) : RuntimeVal("bool"), value(value) {};
    std::string toString() override { return value ? "true" : "false"; };
    bool getValue() { return value; };
};


struct StructValue : public RuntimeVal {
    std::map<std::string, std::shared_ptr<RuntimeVal>> properties;

    StructValue(std::string structName) : RuntimeVal(structName) {}

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
