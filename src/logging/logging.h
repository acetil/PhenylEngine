#ifndef LOGGING_H
#define LOGGING_H

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
namespace logging {
    void log (int level, const char* log);
    void logf(int level, const char* log, ...);
}
#endif