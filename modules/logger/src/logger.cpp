#include <logging/logger.h>

#include "log_manager.h"

using namespace phenyl::logging;

void Logger::initSink() {
    if (logSink) {
        return;
    }

    logSink = LOG_MANAGER.makeSink(this, parent);
}

Logger::Logger (std::string name) : name{std::move(name)}, parent{nullptr} {}

Logger::Logger (std::string name, Logger& parent) : name{std::move(name)}, parent{&parent} {}

void Logger::setMinLevel (const int level) {
    minLogLevel = level;
}

