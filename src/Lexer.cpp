#include "include/Lexer.h"


std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case Identifier:         return "Identifier";
        case TypeIdentifier:      return "TypeIdentifer";
        case Keyword:            return "Keyword";
        case Equals:             return "Equals";
        case Plus:               return "Plus";
        case Asterik:            return "Asterik";
        case Minus:              return "Minus";
        case Division:           return "Division";
        case Modulo:              return "Modulo";
        case Operator:           return "Operator";
        case NumberLiteral:      return "NumberLiteral";
        case StringLiteral:      return "StringLiteral";
        case CharacterLiteral:   return "CharacterLiteral";
        case BooleanLiteral:     return "BooleanLiteral";
        case LessThan:     return "LessThan";
        case GreaterThan:     return "GreaterThan";
        case LParen:             return "LParen";
        case RParen:             return "RParen";
        case LBracket:           return "LBracket";
        case RBracket:           return "RBracket";
        case LBrace:           return "LBrace";
        case RBrace:           return "RBrace";
        case Colon:           return "Colon";
        case Comma:              return "Comma";
        case Semicolon:          return "Semicolon";
        case Dot:                return "Dot";
        case AssignmentOperator: return "AssignmentOperator";
        case ComparisonOperator: return "ComparisonOperator";
        case LogicalOperator:    return "LogicalOperator";
        case EndOfFile:          return "EndOfFile";
        case Unknown:            return "Unknown";
        case Whitespace:         return "Whitespace";
        case SingleLineComment:  return "SingleLineComment";
        case MultiLineComment:   return "MultiLineComment";
        default:                 return "UnknownType";
    }
}
