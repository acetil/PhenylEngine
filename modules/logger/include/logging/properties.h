#pragma once

#include <string>
#include <unordered_map>

#include "logging.h"

namespace phenyl::logging {
    class LoggingProperties {
    private:
        std::unordered_map<std::string, int> m_logLevels;
        std::string m_logFile = "debug.log";
        int m_rootLogLevel = PHENYL_DEFAULT_ROOT_LEVEL;

        friend void phenyl::InitLogging(const LoggingProperties&);
    public:
        LoggingProperties& withLogFile (std::string logFile);
        LoggingProperties& withLogLevel (const std::string& logger, int level);
        LoggingProperties& withRootLogLevel (int level);
    };
}