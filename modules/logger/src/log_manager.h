#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "logging/logger.h"

namespace phenyl::logging {
    namespace detail {
        struct StringHasher {
            using is_transparent = void;

            auto operator() (const std::string& val) const {
                return std::hash<std::string>{}(val);
            }

            auto operator() (const std::string_view val) const {
                return std::hash<std::string_view>{}(val);
            }
        };
    }

    class LogManager {
    private:
        std::unordered_map<std::string, std::unique_ptr<LogSink>, detail::StringHasher, std::equal_to<>> m_sinks;
        std::unordered_map<std::string, int, detail::StringHasher, std::equal_to<>> m_logLevels;
        std::unordered_map<std::string, Logger*, detail::StringHasher, std::equal_to<>> m_loggers;

        std::ofstream m_logFile;
        int m_rootLogLevel = LEVEL_FATAL;
    public:
        void init (int rootLogLevel, const std::string& logPath);
        LogSink* getSink (const Logger* logger, Logger* parent);
        LogSink* getSink (std::string_view path);

        void setLogLevel (const std::string& loggerPath, int level, bool propagate=true);
        void shutdownLogging ();
    };

    extern LogManager LOG_MANAGER;
}
