#pragma once
#include <variant>
#include <string>
#include <stdexcept>

/* struct RuntimeVal { */
/*     // Define a variant type that can hold an int, bool, or string */
/*     std::variant<std::monostate, int, bool, std::string> value; */
/*  */
/*     // Constructors for different types */
/*     RuntimeVal(int intValue) : value(intValue) {} */
/*     RuntimeVal(bool boolValue) : value(boolValue) {} */
/*     RuntimeVal(const std::string& stringValue) : value(stringValue) {} */
/*     RuntimeVal() : value(std::monostate()) {} */
/*  */
/*     // Helper methods to get the value */
/*     int getInt() const { */
/*         if (std::holds_alternative<int>(value)) { */
/*             return std::get<int>(value); */
/*         } */
/*         throw std::runtime_error("RuntimeVal does not hold an int"); */
/*     } */
/*  */
/*     bool getBool() const { */
/*         if (std::holds_alternative<bool>(value)) { */
/*             return std::get<bool>(value); */
/*         } */
/*         throw std::runtime_error("RuntimeVal does not hold a bool"); */
/*     } */
/*  */
/*     std::string getString() const { */
/*         if (std::holds_alternative<std::string>(value)) { */
/*             return std::get<std::string>(value); */
/*         } */
/*         throw std::runtime_error("RuntimeVal does not hold a string"); */
/*     } */
/*  */
/*  */
/*     // Type checking methods */
/*     bool isNull() const { return std::holds_alternative<std::monostate>(value); } */
/*     bool isInt() const { return std::holds_alternative<int>(value); } */
/*     bool isBool() const { return std::holds_alternative<bool>(value); } */
/*     bool isString() const { return std::holds_alternative<std::string>(value); } */
/*  */
/*  */
/*     // Conversion to string for printing or concatenation */
/*     std::string toString() const { */
/*         if (isNull()) return "null"; */
/*         if (isInt()) return std::to_string(getInt()); */
/*         if (isBool()) return getBool() ? "true" : "false"; */
/*         if (isString()) return getString(); */
/*         throw std::runtime_error("Unsupported type for toString"); */
/*     } */
/*  */
/*     std::string typeToString(){ */
/*         if (isNull()) return "null"; */
/*         if (isInt()) return "int"; */
/*         if (isBool()) return "bool"; */
/*         if (isString()) return "string"; */
/*         throw std::runtime_error("Unsupported type for typeToString"); */
/*     } */
/* }; */
/*  */



struct RuntimeVal {
  std::string type;

  RuntimeVal(std::string typeStr): type(typeStr){};
  virtual ~RuntimeVal() = default;
  virtual std::string toString() = 0;
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
