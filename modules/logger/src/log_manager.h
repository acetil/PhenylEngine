#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "logging/logger.h"

namespace phenyl::logging {
    class LogManager {
    private:
        std::unordered_map<std::string, std::unique_ptr<LogSink>> sinks;
        std::unordered_map<std::string, int> logLevels;
        std::unordered_map<std::string, Logger*> loggers;

        std::ofstream logFile;
        int rootLogLevel = LEVEL_FATAL;
    public:
        void init (int rootLogLevel, const std::string& logPath);
        LogSink* makeSink (Logger* logger, Logger* parent);
        void setLogLevel (const std::string& loggerPath, int level);
        void shutdownLogging ();
    };

    extern LogManager LOG_MANAGER;
}
