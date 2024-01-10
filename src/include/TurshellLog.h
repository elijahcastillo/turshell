
#pragma once
#include <string>
#include <iostream>
#include <sstream>   // For std::ostringstream



class TurshellLog {
public:
    static int settings; // Make settings static

    enum Flags {
        LOG_INFO = 1,         // 0001 in binary
        LOG_MEMORY_INFO = 2,  // 0010 in binary
        LOG_TEST = 4          // 0100 in binary
        // You can define more flags here
    };



    static void setSettings(int newSettings) {
        settings = newSettings; // Use the static settings variable
    }

    static void Log(const std::string& message, int flag) {
        if (settings & flag) {
            std::cout << "Turshell Log: " << message << "\n";
        }
    }
};



std::string pointerAddrToString(const void* ptr);

std::string logFlagToString(TurshellLog::Flags flag);


