#include "stdarg.h"
#ifndef LOGGING_INTERNAL_H
#define LOGGING_INTERNAL_H

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
namespace internal {
    void log_internal (int level, const char* location, const char* log);
    void vlogf_internal(int level, const char* locationl, const char* log, va_list l);
}
#endif