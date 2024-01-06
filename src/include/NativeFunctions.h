#pragma once
#include "Lexer.h"
#include "Runtime.h"
#include "Interpretor.h"
#include <iostream>
#include <memory>
#include <stdexcept>

// Example of a native function
std::shared_ptr<RuntimeVal> nativePrint(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args);


std::shared_ptr<RuntimeVal> nativeInput(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args);

std::shared_ptr<RuntimeVal> nativeAppend(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args);





std::shared_ptr<RuntimeVal> nativeLen(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args);

std::shared_ptr<RuntimeVal> nativeMathAbs(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args);


std::shared_ptr<RuntimeVal> nativeFileRead(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args);


std::shared_ptr<RuntimeVal> nativeMathSqrt(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args);


std::shared_ptr<RuntimeVal> nativeMathPow(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args);


std::shared_ptr<RuntimeVal> nativeRandom(Interpreter& interpreter, std::vector<std::shared_ptr<RuntimeVal>>& args);
