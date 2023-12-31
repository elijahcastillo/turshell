#pragma once


#include <ctype.h>
#include <string>
#include <iostream>
#include <set>
#include <vector>


enum TokenType {
    Identifier,          // Variable names, function names, etc. Example: "foo", "var1"
    TypeIdentifier,       // int, string
    Keyword,             // Language keywords. Example: "if", "while", "return"
    Equals,               // =
    Plus,                 // +
    Asterik,              // *
    Minus,                // -
    Division,             // /
    Modulo,                // %
    Operator,             // + - / *
    NumberLiteral,       // Numeric literals. Example: "123", "3.14"
    StringLiteral,       // String literals. Example: "\"Hello, world!\""
    CharacterLiteral,    // Character literals. Example: "'a'", "'3'"
    BooleanLiteral,      // Boolean literals. Example: "true", "false"
    LParen,              // (
    RParen,              // ) 
    LBracket,            // { 
    RBracket,            // } 
    Comma,               // Comma for separation. Example: ","
    Semicolon,           // Semicolon as a statement terminator. Example: ";"
    Dot,                 // Dot for object property access. Example: "."
    AssignmentOperator,  // Assignment operator. Example: "=", "+=", "-="
    ComparisonOperator,  // Comparison operators. Example: "==", "!=", "<", ">"
    LogicalOperator,     // Logical operators. Example: "&&", "||", "!"
    EndOfFile,           // Special token indicating the end of the file.
    Unknown,             // Token type for unrecognized symbols.
    Whitespace,          // Spaces, tabs, newlines.
    SingleLineComment,   // Single-line comments. Example: "// This is a comment"
    MultiLineComment     // Multi-line comments. Example: "/* Comment */"
};



std::string tokenTypeToString(TokenType type);

struct Token {
  TokenType type;
  std::string value;
  size_t lineNumber;

  Token(TokenType type, std::string value, size_t lineNumber): type(type), value(value), lineNumber(lineNumber){};
  Token(): type(Unknown), value(""), lineNumber(0){};

  void print(){
std::cout << "{  type: " << tokenTypeToString(type) << ", value: " << value << ", line: " << lineNumber << "  }";

    
  }
};

class Tokenizer {
  std::string source;
  size_t position;
  size_t lineNumber;  // Add a line number member

public:
  Tokenizer(const std::string& source): source(source), position(0), lineNumber(1){};

  void skipWhitespace(){
    while(position < source.size() && isspace(source[position])){
        if (source[position] == '\n') {
            lineNumber++;  // Increment line number on new line
        }
      position++;
    }
  }


  Token nextToken(){
    skipWhitespace();

    //EOF
    if(position >= source.size()){
      return Token(EndOfFile, "", lineNumber);
    }

    char currentChar = source[position];


    // Identifier or keyword
    if (isalpha(currentChar) || currentChar == '_') {
        return consumeIdentifierOrKeyword();
    }

    // Number literal
    if (isdigit(currentChar)) {
        return consumeNumber();
    }

    // String literal
    if (currentChar == '\"') {
        return consumeString();
    }

    // Single character token (operator, punctuation)
    return consumeSingleCharacterToken();

  }


  Token consumeIdentifierOrKeyword() {
    size_t start = position;
    while(position < source.size() && (isalpha(source[position]) || source[position] == '_')){
      position++;
    }

    std::string identifier = source.substr(start, position - start);

    //Check if identifier is keyword
    if(isKeyword(identifier)){
      return Token(Keyword, identifier, lineNumber);
    }

    //Check if type Identifier
    if(isType(identifier)){
      return Token(TypeIdentifier, identifier, lineNumber);
    }

    return Token(Identifier, identifier, lineNumber);
    
  }

bool isKeyword(const std::string& identifier) {
    static const std::set<std::string> keywords = {"if", "while", "return", "func"};
    return keywords.find(identifier) != keywords.end();  
}


bool isType(const std::string& identifier) {
    static const std::set<std::string> types = {"int", "string"};
    return types.find(identifier) != types.end();  
}


Token consumeNumber() {
    size_t start = position;
    while (position < source.size() && isdigit(source[position])) {
        position++;
    }

    // Handle floating point numbers
    if (position < source.size() && source[position] == '.') {
        position++;
        while (position < source.size() && isdigit(source[position])) {
            position++;
        }
    }

    std::string number = source.substr(start, position - start);
    return {NumberLiteral, number, lineNumber};
}


Token consumeString() {
    position++; // Skip the opening quote
    size_t start = position;

    while (position < source.size() && source[position] != '\"') {
        // Handle escape characters here if your language supports them
        position++;
    }

    if (position >= source.size()) {
        throw std::runtime_error("Unterminated string literal at " + std::to_string(lineNumber));
    }

    std::string str = source.substr(start, position - start);
    position++; // Skip the closing quote
    return {StringLiteral, str, lineNumber};
}


Token consumeSingleCharacterToken() {
    char currentChar = source[position++];
    char nextChar = (position < source.size()) ? source[position] : '\0';
    TokenType type = Unknown;

    if(currentChar == '=' && nextChar == '='){
      position++; //Skip second '='
      return {ComparisonOperator, "==", lineNumber};
    }

    if(currentChar == '>' && nextChar == '='){
      position++; //Skip second '='
      return {ComparisonOperator, ">=", lineNumber};
    }

    if(currentChar == '<' && nextChar == '='){
      position++; //Skip second '='
      return {ComparisonOperator, "<=", lineNumber};
    }


    switch (currentChar) {
        case '=': type = Equals; break;
        case '+': type = Plus; break;
        case '-': type = Minus; break;
        case '*': type = Asterik; break;
        case '/': type = Division; break;
        case '%': type = Modulo; break;
        case '>': type = ComparisonOperator; break;
        case '<': type = ComparisonOperator; break;
        case '(': type = LParen; break;
        case ')': type = RParen; break;
        case '{': type = LBracket; break;
        case '}': type = RBracket; break;
        case ',': type = Comma; break;
        case ';': type = Semicolon; break;
        case '.': type = Dot; break;
        // Add more cases for other single-character tokens
        default: type = Unknown; break;
    }

    return {type, std::string(1, currentChar), lineNumber};
}

  
};
