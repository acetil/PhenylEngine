#include <chrono>
#include <ranges>
#include <utility>

#include "logging/log_sink.h"
#include "logging/logger.h"
#include "logging/logging.h"

#define TRACE_TEXT "TRACE"
#define DEBUG_TEXT "DEBUG"
#define INFO_TEXT "INFO"
#define WARNING_TEXT "WARN"
#define ERROR_TEXT "ERROR"
#define FATAL_TEXT "FATAL"

using namespace phenyl::logging;

static constexpr std::string_view GetLevelText (const int level) {
    switch (level) {
        case LEVEL_TRACE:
            return TRACE_TEXT;
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

LogSink::LogSink (std::string path) : m_path{std::move(path)} {}

void LogSink::log (const std::source_location sourceLoc, const int level, const std::string& logText) {
    auto now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());

    log(std::format("[{:%H:%M:%S}] ({}/{}) [{}]", now, m_name, GetLevelText(level), sourceLoc.function_name()), logText);
}

void LogSink::setMinLogLevel (const int logLevel, const bool propagate) {
    PHENYL_ASSERT(logLevel >= LEVEL_TRACE && logLevel <= LEVEL_FATAL);

    m_minLogLevel = logLevel;

    if (propagate) {
        for (auto* child : m_children) {
            child->setMinLogLevel(logLevel);
        }
    }
}

void LogSink::setName (std::string name) {
    m_name = std::move(name);
}

void LogSink::addChild (LogSink* childSink) {
    m_children.emplace_back(childSink);
}

