#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "logging_internal.h"
#define TIME_TEXT_LEN 21
using namespace phenyl::internal;
char buffer[2000];

#ifdef C_LOGGING
class LogFile {
private:
    FILE* file;
public:
    LogFile() noexcept {
        file = nullptr;
    }
    explicit LogFile (const std::string& path) {
        printf("Logging path: %s\n", path.c_str());
        file = fopen(path.c_str(), "w");
        if (!file) {
            fprintf(stderr, "Error opening log file \"%s\"!", path.c_str());
        }
    }
    void writeLine (const char* timeText, const char* location, const char* text, const char* log) {
#ifdef DEBUG_LOG
        if (!file) {
            printf("Logger not initialised!");
            return;
        }
#endif
        auto res = fprintf(file, "%s (%s/%s): %s\n", timeText, location, text, log);
#ifdef DEBUG_LOG
        if (res < 0) {
            printf("Error writing to log file: %d, %d\n", res, errno);
        }

        printf("%s (%s/%s): %s\n", timeText, location, text, log);
        fflush(file);
#endif
    }

    LogFile& operator= (LogFile&& other) noexcept {
        file = other.file;
        other.file = nullptr;
        return *this;
    }

    ~LogFile() {
        if (file) {
            fclose(file);
        }
    }
};
#else
#ifdef CPP_LOGGING
#include <iostream>
#include <fstream>
class LogFile {
private:
    std::ofstream file;
public:
    LogFile () = default;
    explicit LogFile (const std::string& path) {
        std::cout << "Logging path (cpp): " << path << "\n";
        file = std::ofstream(path);
    }

    void writeLine (const std::string& timeText, const std::string& location, const std::string& text, const std::string& log) {
        if (!file) {
            std::cout << "Logger not initialised!\n";
        }

        file << timeText << " (" << location << "/" << text << ": " << log;
#ifdef DEBUG_LOG
        file << std::endl;
        std::cout << timeText << " (" << location << "/" << text << ": " << log << "\n";
#else
        file << "\n";
#endif
    }

    LogFile& operator= (LogFile&& other) noexcept {
        file = std::move(other.file);
        return *this;
    }
};
#else
class LogFile {
public:
    LogFile() = default;
    explicit LogFile (const std::string&) {}
    void writeLine (const char* timeText, const char* location, const char* text, const char* log) {
        printf("%s (%s/%s): %s\n", timeText, location, text, log);
    }
};
#endif
#endif

LogFile logFile{};

void phenyl::internal::log_internal (int level, const char* location, const char* log) {
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

    logFile.writeLine(timeText, location, text, log);
}
void phenyl::internal::vlogf_internal (int level, const char* location, const char* log, va_list l) {
    vsprintf(buffer, log, l);
    log_internal(level, location, buffer);
}

void phenyl::internal::log_internal (int level, const std::string& location, const std::string& log) {
    log_internal(level, location.c_str(), log.c_str());
}

void phenyl::internal::init_internal (const std::string& logPath) {
    logFile = LogFile(logPath);
}