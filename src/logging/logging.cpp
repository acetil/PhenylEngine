#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "logging.h"
#define TIME_TEXT_LEN 21
using namespace logging;
void logging::log (int level, const char* log) {
    //TODO: update to print to log file
    const char* text;
    char timeText[TIME_TEXT_LEN];
    switch (level) {
        case LEVEL_WARNING:
            text = WARNING_TEXT;
            break;
        case LEVEL_ERROR:
            text = ERROR_TEXT;
            break;
        case LEVEL_FATAL:
            text = FATAL_TEXT;
            break;
        case LEVEL_INFO:
        default:
            text = LEVEL_INFO;
            break;
    }
    time_t now = time(0);
    strftime(timeText, TIME_TEXT_LEN, "[%T]", localtime(&now));
    printf("%s (%s): %s\n", text, timeText, log);
}

void logging::logf(int level, const char* logFormat, ...) {
    va_list argPtr;
    va_start(argPtr, logFormat);
    char buffer[200];
    vsprintf(buffer, logFormat, argPtr);
    log(level, buffer);
}