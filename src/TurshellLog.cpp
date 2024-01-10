#include "include/TurshellLog.h"
// Initialize the static variable
int TurshellLog::settings = 0;

std::string pointerAddrToString(const void* ptr) {
    std::ostringstream stream;
    stream << ptr;
    return stream.str();
}
