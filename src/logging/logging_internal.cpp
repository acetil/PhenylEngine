#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "logging_internal.h"
#define TIME_TEXT_LEN 21
using namespace internal;
char buffer[2000];
void internal::log_internal (int level, const char* location, const char* log) {
    //TODO: update to print to log file
    #ifndef DEBUG_LOG
        if (level == LEVEL_DEBUG) {
            return;
        }
    #endif
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
        case LEVEL_DEBUG:
            text = DEBUG_TEXT;
            break;
        case LEVEL_INFO:
        default:
            text = INFO_TEXT;
            break;
    }
    time_t now = time(nullptr);
    strftime(timeText, TIME_TEXT_LEN, "[%H:%M:%S]", localtime(&now));
    printf("%s (%s/%s): %s\n", timeText, location, text, log);
}
void internal::vlogf_internal (int level, const char* location, const char* log, va_list l) {
    vsprintf(buffer, log, l);
    log_internal(level, location, buffer);
}
