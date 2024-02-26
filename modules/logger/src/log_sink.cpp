#include <chrono>

#include <logging/log_sink.h>
#include <logging/logger.h>

#define DEBUG_TEXT "DEBUG"
#define INFO_TEXT "INFO"
#define WARNING_TEXT "WARN"
#define ERROR_TEXT "ERROR"
#define FATAL_TEXT "FATAL"

using namespace phenyl::logging;

static constexpr std::string_view GetLevelText (const int level) {
    switch (level) {
        case LEVEL_DEBUG:
            return DEBUG_TEXT;
        case LEVEL_INFO:
            return INFO_TEXT;
        case LEVEL_WARN:
            return WARNING_TEXT;
        case LEVEL_ERROR:
            return ERROR_TEXT;
        case LEVEL_FATAL:
            return FATAL_TEXT;
        default:
            [[unlikely]]
            return "UNKNOWN";
    }
}

LogSink::LogSink (std::string name, std::string path) : sinkName{std::move(name)}, sinkPath{std::move(path)} {}

void LogSink::log (const std::source_location sourceLoc, const int level, const std::string& logText) {
    auto now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());

    log(std::format("[{:%H:%M:%S}] ({}/{}) [{}]", now, sinkName, GetLevelText(level), sourceLoc.function_name()), logText);
}

