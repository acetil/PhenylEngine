#pragma once

#include <string>
#include "stdarg.h"

#define LEVEL_INFO 0
#define LEVEL_WARNING 1
#define LEVEL_ERROR 2
#define LEVEL_FATAL 3
#define LEVEL_DEBUG 4

#define INFO_TEXT "INFO"
#define WARNING_TEXT "WARNING"
#define ERROR_TEXT "ERROR"
#define FATAL_TEXT "FATAL"
#define DEBUG_TEXT "DEBUG"
namespace phenyl::internal {
    void log_internal (int level, const char* location, const char* log);
    void log_internal (int level, const std::string& location, const std::string& log);
    void vlogf_internal(int level, const char* location, const char* log, va_list l);

    void init_internal (const std::string& logPath);
}