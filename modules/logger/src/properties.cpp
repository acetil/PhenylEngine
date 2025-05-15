#include "logging/properties.h"

#include "log_manager.h"

using namespace phenyl::logging;

LoggingProperties& LoggingProperties::withLogFile (std::string logFile) {
    this->m_logFile = std::move(logFile);

    return *this;
}

LoggingProperties& LoggingProperties::withLogLevel (const std::string& logger, const int level) {
    m_logLevels[logger] = level;

    return *this;
}

LoggingProperties& LoggingProperties::withRootLogLevel (const int level) {
    m_rootLogLevel = level;

    return *this;
}
