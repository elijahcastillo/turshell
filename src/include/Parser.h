#pragma once
#include <stdexcept>
#include <string>
#include <vector>
#include "AST_Types.h"
#include "Lexer.h"

class Parser {
    std::vector<Token> tokens;
    size_t current;

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    ASTNode* parse() {
      std::vector<ASTNode*> statements;

      while (!isAtEnd()) {
          statements.push_back(parseStatement());
       }

      return new ProgramNode(statements);
    }

private:
    // Parsing methods for different constructs
    //


    std::string getAnyType(){
      std::string type = "Unkown Type";
      if(check(TokenType::TypeIdentifier) || check(TokenType::Identifier)){
        if(peek().value == "array"){
          type = getArrayType();
        } else {
          type = advance().value;
        }
      }
      return type;
    }

    ASTNode* parseStatement(){


      //KEYWORDS
      if(check(TokenType::Keyword)){
        Token token = advance();

        if(token.value == "true"){
          return new BinaryLiteralNode(true);
        }

        if(token.value == "false"){
          return new BinaryLiteralNode(false);
        }

        //Parse if statment Ex: if(a == 3){}
        if(token.value == "if"){
          return parseIfStatement();
        }

        if(token.value == "while"){
          return parseWhileStatement();
        }

        if(token.value == "func"){
          return parseFunctionDeclaration();
        }

        if(token.value == "impl"){
          std::string structName = consume(TokenType::Identifier, "Expected struct name for impl").value;
          consume(TokenType::Minus, "Expected '->' after struct impl  Ex: impl Struct -> ...");
          consume(TokenType::GreaterThan, "Expected '->' after struct impl  Ex: impl Struct -> ...");

          ASTNode* functionDeclaration = parseFunctionDeclaration();

          return new StructMethodDeclarationNode(structName, functionDeclaration);
        }



        if(token.value == "struct"){
          return parseStruct();
        }

        if(token.value == "break"){
          consume(TokenType::Semicolon, "Expect ';' after break");
          return new BreakStatementNode();
        }

        // Parse return statements
        if (token.value == "return") {
            /* std::cout << "RETURNRNRN\n"; */
            ASTNode* expression = nullptr;
            if (!check(TokenType::Semicolon)) {
                expression = parseExpression();
            }
            /* std::cout << "AFFFFF: " << peek().value << std::endl; */
            consume(TokenType::Semicolon, "Expect ';' after return value");

            return new ReturnStatementNode(expression);
        }
      } 

      //Parse Variable Declaration Ex: int a = 3 + 2;
     if(check(TokenType::TypeIdentifier)){
       std::string varType;
       std::string varName;

        if (peek().value == "array") {
            varType = getArrayType();
            varName = consume(TokenType::Identifier, "Expected Name for variable").value;

            return parseVariableDeclaration(varType, varName, true);
        }

        varType = consume(TokenType::TypeIdentifier, "Expected Type").value;
        varName = consume(TokenType::Identifier, "Expected Name for variable").value;

        return parseVariableDeclaration(varType, varName);
     } 



    //Parse Variable with non-primtive type
      if (check(TokenType::Identifier)) {
          std::string potentialType = peek().value;

          // Assume an identifier at the start of a statement could be a type name, user defined
          advance(); // Move past the identifier

          if (check(TokenType::Identifier)) {
              std::string varName = consume(TokenType::Identifier, "Expected variable name").value;

              return parseVariableDeclaration(potentialType, varName);
          } else {
              // If the next token is not an Identifier, backtrack and parse it as a regular expression/statement
              current--; // Move back to the potential type identifier
              return parseExpression();
          }
      }


     return parseExpression();
    }



std::string getArrayType() {
    if (peek().value != "array") {
        return "THIS IS BAD ARRAY THING"; // Or throw an error
    }

    advance(); // Consume array keyword
    consume(TokenType::LessThan, "Expected '<' after 'array'");

    std::string elementType;

    // Check for nested array
    if (peek().value == "array") {
        elementType = getArrayType(); // Recursive call to handle nested arrays
    } else if (check(TokenType::TypeIdentifier) || check(TokenType::Identifier)) {
        elementType = advance().value; // Consume the type identifier
    } else {
        return "THIS IS BAD ARRAY THING"; // Or throw an error
    }


    consume(TokenType::GreaterThan, "Expected '>' after type identifier");

    return "array<" + elementType + ">";
}



    ASTNode* parseVariableDeclaration(std::string& varType, std::string& varName, bool isArray = false){

      /* std::cout << "Parsing varible declartion: " << varType << " " << varName << "\n"; */
        ASTNode* varInitializer = nullptr;

        if(match(TokenType::Equals)){

          varInitializer = parseExpression();
        }



        consume(TokenType::Semicolon, "Expect ';' after variable declaration");

        if(varInitializer == nullptr){
          throw std::invalid_argument("Varaible " +varName+" must be initialized: line "+std::to_string(previous().lineNumber));
        }

        return new VariableDeclarationNode(varName, varType, varInitializer, isArray);
    }



    //struct Point {
    //  int x;
    //  int y;
    //};
    ASTNode* parseStruct(){
      std::string structName = consume(TokenType::Identifier, "Expected identifer after struct keyword").value;
      consume(TokenType::LBrace, "Expected '{' after struct identifier");

      //Parse properties of the struct
      std::vector<ASTNode*> properties;

      if(!check(TokenType::LBrace)){

        do{

          if(peek().type == TokenType::RBrace) break;

          std::string paramType;
          if(check(TokenType::Identifier) || check(TokenType::TypeIdentifier)){
            paramType = getAnyType();
          } else {
              throw std::invalid_argument("Expected struct property type");
          }

          std::string paramName = consume(TokenType::Identifier, "Expected struct property name").value;
          properties.push_back(new ParameterNode(paramType, paramName));
        } while(match(TokenType::Semicolon));

      }

      if(properties.size() == 0){
        throw std::invalid_argument("Struct must have atleast one property");
      }

      consume(TokenType::RBrace, "Expected '}' after struct");
      consume(TokenType::Semicolon, "Expected ';' after struct");
      return new StructDeclarationNode(structName, properties);

    }


    // func add(int a, int b) int {
    //  *body
    //}
    ASTNode* parseFunctionDeclaration(){
      
      std::string functionName = consume(TokenType::Identifier, "Expected function name").value;
      consume(TokenType::LParen, "Expected '(' after function name");

      std::vector<ASTNode*> parameters;

      //If () is empty dont parse parameters
      if(!check(TokenType::RParen)){
        do{

          std::string paramType = getAnyType();
          /* std::string paramType = consume(TokenType::TypeIdentifier, "Expected parameter type").value; */
          std::string paramName = consume(TokenType::Identifier, "Expected parameter name").value;
          parameters.push_back(new ParameterNode(paramType, paramName));
        } while(match(TokenType::Comma));

      }


      consume(TokenType::RParen, "Expected ')' after parameters");
      std::string returnType = getAnyType();
      ASTNode* body = parseBlock();

      return new FunctionDeclarationNode(returnType, functionName, parameters, body);

    }


    ASTNode* parseIfStatement() {
        consume(TokenType::LParen, "Expected '(' after 'if'");
        ASTNode* condition = parseExpression();
        consume(TokenType::RParen, "Expected ')' after condition");

        ASTNode* thenBranch = parseBlock();
        ASTNode* elseBranch = nullptr;

        if (check(TokenType::Keyword) && peek().value == "else") {
            advance(); // Consume 'else'
            elseBranch = parseBlock();
        }


        return new IfStatementNode(condition, thenBranch, elseBranch);
    }

    ASTNode* parseWhileStatement() {
        consume(TokenType::LParen, "Expected '(' after 'while'");
        ASTNode* condition = parseExpression();
        consume(TokenType::RParen, "Expected ')' after condition");
        ASTNode* body = parseBlock();

        return new WhileStatementNode(condition, body);
    }


    ASTNode* parseBlock() {
        consume(TokenType::LBrace, "Expected '{' at the start of block");

        std::vector<ASTNode*> statements;
        while (!check(TokenType::RBrace) && !isAtEnd()) {
            statements.push_back(parseStatement());
        }

        consume(TokenType::RBrace, "Expected '}' at the end of block");

        return new BlockNode(statements);
    }


    ASTNode* parseArrayLiteral() {
        consume(TokenType::LBracket, "Expected '[' at the beginning of array literal");
        std::vector<ASTNode*> elements;
        if (!check(TokenType::RBracket)) {
            do {
                elements.push_back(parseExpression());
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RBracket, "Expected ']' at the end of array literal");
        return new ArrayLiteralNode(elements);
    }



    ASTNode* parseStructInitializerList() {
        consume(TokenType::LBrace, "Expected '{' at the beginning of struct initializer list");
        std::vector<ASTNode*> initializers;

        do {
            std::string propertyName = consume(TokenType::Identifier, "Expected property name").value;
            consume(TokenType::Colon, "Expected ':' after property name");
            ASTNode* initializer = parseExpression();
            initializers.push_back(new VariableAssignmentNode(propertyName, initializer));
        } while (match(TokenType::Comma));

        consume(TokenType::RBrace, "Expected '}' at the end of struct initializer list");
        return new StructInitalizerListNode(initializers);
    }


    ASTNode* parseExpression() {


      //Struct Initializer list: Ex -  {x: 1+2, y: 5}
      if(check(TokenType::LBrace) && peekNext().type == TokenType::Identifier){
        return parseStructInitializerList();
      }






      //Parse variable assignment Ex: a = 3;
      if(check(TokenType::Identifier) && peekNext().type == TokenType::Equals){
        std::string varName = consume(TokenType::Identifier, "Expected Name of variable").value;
        advance(); // Consume the '='
        ASTNode* value = parseExpression();

        consume(TokenType::Semicolon, "Expect ';' after variable assigment");

        return new VariableAssignmentNode(varName, value);
      }



        return parseLogical();
    }




    ASTNode* parseLogical() {
        ASTNode* node = parseComparison();

        while (check(TokenType::LogicalOperator)) {
            std::string op = advance().value;
            ASTNode* right = parseComparison();
            node = new LogicalOperatorNode(node, op, right);
        }

        return node;
    }

    ASTNode* parseComparison() {
        ASTNode* node = parseAddition();

        while (check(TokenType::ComparisonOperator) || check(TokenType::GreaterThan) || check(TokenType::LessThan)) {
            std::string op = advance().value;
            ASTNode* right = parseAddition();
            node = new BinaryExpressionNode(node, op, right);
        }

        return node;
    }


    // Implement logic to check if the next token is valid after an operator
    // For example, after a '+' token, you expect a number, an identifier, or a '('
    // Return true if the next token is valid, false otherwise
    bool isNextTokenValidAfterOperator() {
        if (isAtEnd()) return false;

        TokenType nextType = peek().type;
        return nextType == TokenType::NumberLiteral || 
               nextType == TokenType::StringLiteral || 
               nextType == TokenType::BooleanLiteral ||
               nextType == TokenType::FloatLiteral||
               nextType == TokenType::Identifier || 
               nextType == TokenType::LParen;
    }

    ASTNode* parseAddition() {


        //Multiplcation has more precidence than addition
        ASTNode* node = parseMultiplication();

        while (check(TokenType::Plus) || check(TokenType::Minus)) {
            Token op = advance();

            if (isAtEnd() || !isNextTokenValidAfterOperator()) {
                throw std::invalid_argument("Syntax Error: Incomplete expression at line " + std::to_string(op.lineNumber));
            }

            /* std::cout << "LOLOLLL: " << peek().value << "\n"; */

            ASTNode* right = parseMultiplication();

            node = new BinaryExpressionNode(node, op.value, right);
        }

        return node;
    }

    ASTNode* parseMultiplication() {
        ASTNode* node = parsePrimary();


        Token currentToken = tokens[current];
        while (check(TokenType::Asterik) || check(TokenType::Division) || check(TokenType::Modulo)) {
            Token op = advance();

            if (isAtEnd() || !isNextTokenValidAfterOperator()) {
                throw std::invalid_argument("Syntax Error: Incomplete expression at line " + std::to_string(op.lineNumber));
            }

            ASTNode* right = parsePrimary();
            node = new BinaryExpressionNode(node, op.value, right);
        }

        return node;
    }

    ASTNode* parseFunctionCall(std::string name) {

        std::string functionName = name;
        std::cout << "INSIDE: " << peek().value << "\n";
        
        consume(TokenType::LParen, "Expected '(' after Identifier for func call");
        std::vector<ASTNode*> arguments;


        // Check if argument list is not empty
        if (!check(TokenType::RParen)) {
            
            do {
                arguments.push_back(parseExpression());
            } while (match(TokenType::Comma));
        
        }


        consume(TokenType::RParen, "Expected ')' after arguments");

        /* std::cout << peek().value << "\n"; */

        //If function call is part of statmet int a = add(3,2); then it would look for another ;
        /* consume(TokenType::Semicolon, "Expected ';' after arguments"); */

        return new FunctionCallNode(functionName, arguments);
    }



ASTNode* parseChainedAccess(const std::string& baseName) {
    std::vector<ASTNode*> accesses;
    accesses.push_back(new VariableExpressionNode(baseName)); // Starting point of the chain

    while (check(TokenType::Dot) || check(TokenType::LBracket) || check(TokenType::LParen)) {
        std::cout << "Checking " << peek().value << "\n";
        if (match(TokenType::Dot)) {
            std::string propertyName = consume(TokenType::Identifier, "Expected property name after '.'").value;

            //Struct function call   a.test()
            if(peek().type == TokenType::LParen){
              ASTNode* funcCall = parseFunctionCall(propertyName);
              accesses.push_back(new StructMethodCallNode(propertyName, funcCall)); // You need to create this node type
              continue;
            }

            accesses.push_back(new PropertyAccessNode(propertyName)); // You need to create this node type
        } else if (match(TokenType::LBracket)) {
            ASTNode* index = parseExpression();
            consume(TokenType::RBracket, "Expected ']' after index");
            accesses.push_back(new ArrayAccessNode(index)); // Reuse your existing ArrayAccessNode
        } 
    }

    return new ChainedAccessNode(accesses);
}



    ASTNode* parsePrimary() {

        //Handle Parenthesis
        if(check(TokenType::LParen)){
          advance(); //Consume '('

          //Parse inside
          ASTNode* node = parseExpression();

          if(!match(TokenType::RParen)){
            // Handle the missing ')' error. 
            throw std::invalid_argument("Syntax Error: Expected ')' after expression: line " + std::to_string(tokens[current].lineNumber));
            /* return nullptr; // Or however you handle errors */
          }

          return node;

        }

        


        
        //Array Literals   [1, 5, 3]
        if (check(TokenType::LBracket)) {
            return parseArrayLiteral();
        }

        //True and False
      if(check(TokenType::Keyword)){
        Token token = advance();

        if(token.value == "true"){
          return new BinaryLiteralNode(true);
        }

        if(token.value == "false"){
          return new BinaryLiteralNode(false);
        }
      }

        // Unary Negation like (-2) or !variable
        if (check(TokenType::Minus) || (check(TokenType::LogicalOperator) && peek().value == "!")) {
          std::string op = advance().value; // Consume the '-'
            ASTNode* right = parsePrimary(); // Recursively parse the right-hand side
            return new UnaryExpressionNode(op, right); // Create a node for the unary negation
        }

        //Positive Numbers
        if (check(TokenType::NumberLiteral)) {
            std::string str_value = advance().value;
            int value = std::stoi(str_value);
            return new IntLiteralNode(value);
        }

        if (check(TokenType::FloatLiteral)) {
            std::string str_value = advance().value;
            float value = std::stof(str_value);
            return new FloatLiteralNode(value);
        }

        if (check(TokenType::StringLiteral)) {
            std::string str_value = advance().value;
            return new StringLiteralNode(str_value);
        }



    // Handle Chained Access or Assignment   struct.names[1].age
    /* if (check(TokenType::Identifier) && (peekNext().type == TokenType::Dot || peekNext().type == TokenType::LBracket)) { */
    /*     ASTNode* access = parseChainedAccess(consume(TokenType::Identifier, "Expected identifier").value); */
    /*  */
    /*     if (match(TokenType::Equals)) { */
    /*         ASTNode* value = parseExpression(); */
    /*         consume(TokenType::Semicolon, "Expect ';' after assignment"); */
    /*         return new ChainedAssignmentNode(access, value); */
    /*     } */
    /*  */
    /*     return access; */
    /* } */


    // Handle Identifiers (e.g., variable names or function calls)
    if (check(TokenType::Identifier)) {
        std::string baseName = advance().value;


        // Check for chained access
        if (peek().type == TokenType::Dot || peek().type == TokenType::LBracket) {

            ASTNode* access = parseChainedAccess(baseName);

            if (match(TokenType::Equals)) {
                ASTNode* value = parseExpression();
                consume(TokenType::Semicolon, "Expect ';' after assignment");
                return new ChainedAssignmentNode(access, value);
            }

            return access;
        }

        // Check for function call (identifier followed by '(')
        if (peek().type == TokenType::LParen) {
            std::cout << "Noraml Func call\n";
            return parseFunctionCall(baseName);
        }

        return new VariableExpressionNode(baseName); // You need to create this node type
    }



       if(peek().type == TokenType::EndOfFile){
       
           return nullptr;
       }
       
        throw std::invalid_argument("Unknown Language Grammer: Cannot Parse Token of " + peek().value + " " + std::to_string(peek().lineNumber) );

        // Handle other primary types (like parentheses) here
    }

    // Utility methods
    bool match(TokenType expected) {
        if (check(expected)) {
            advance();
            return true;
        }
        return false;
    }

    bool checkValue(std::string value){
      
        if (isAtEnd()) return false;
        return tokens[current].value == value;
    }

    bool check(TokenType type) {
        if (isAtEnd()) return false;
        return tokens[current].type == type;
    }

    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    bool isAtEnd() {
        return peek().type == EndOfFile;
    }

    Token consume(TokenType type, std::string err) {
        if (check(type)) {
            return advance();
        }
        throw std::invalid_argument(err + ": line " + std::to_string(peek().lineNumber));
    }

    Token peekNext(){
      return tokens[current + 1];
    }

    Token peek() {
        return tokens[current];
    }

    Token previous() {
        return tokens[current - 1];
    }

    // More utility methods like expect, consume, error handling, etc.
};
