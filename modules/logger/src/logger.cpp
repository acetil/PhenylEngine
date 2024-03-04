#include "logging/logger.h"

#include "log_manager.h"

using namespace phenyl::logging;

void Logger::initSink() {
    if (logSink) {
        return;
    }

    logSink = LOG_MANAGER.getSink(this, parent);
}

Logger::Logger (const std::string_view name) : name{name}, parent{nullptr} {}

Logger::Logger (const std::string_view name, Logger& parent) : name{name}, parent{&parent} {}

void Logger::setMinLevel (const int level) {
    logSink->setMinLogLevel(level);
}

