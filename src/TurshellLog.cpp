#include "include/TurshellLog.h"
// Initialize the static variable
int TurshellLog::settings = 0;

std::string pointerAddrToString(const void* ptr) {
    std::ostringstream stream;
    stream << ptr;
    return stream.str();
}

std::string logFlagToString(TurshellLog::Flags flag){
  switch (flag) {
    case TurshellLog::LOG_INFO: return "LOG_INFO";
    case TurshellLog::LOG_MEMORY_INFO: return "LOG_MEM_INFO";
    case TurshellLog::LOG_TEST: return "LOG_TEST";
  }
}
