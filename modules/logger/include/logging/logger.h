#pragma once

#include <format>
#include <string>

#include "forward.h"
#include "log_sink.h"

namespace phenyl::logging {
    class Logger {
    private:
        std::string name;
        int minLogLevel = LEVEL_DEBUG;
        Logger* parent;
        LogSink* logSink = nullptr;

        void initSink ();

        void setMinLevel (int level);

        friend class LogManager;
    public:
        explicit Logger (std::string name);
        Logger (std::string name, Logger& parent);

        template <typename ...Args>
        void log (const std::source_location sourceLoc, const int level, std::format_string<Args...> fmt, Args&&... args) {
            if (level < minLogLevel) {
                return;
            }

            if (!logSink) {
                [[unlikely]]
                initSink();
            }

            logSink->log(sourceLoc, level, std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename ...Args>
        void debug (const std::source_location sourceLoc, std::format_string<Args...> fmt, Args&&... args) {
            log(sourceLoc, LEVEL_DEBUG, std::move(fmt), std::forward<Args>(args)...);
        }

        template <typename ...Args>
        void info (const std::source_location sourceLoc, std::format_string<Args...> fmt, Args&&... args) {
            log(sourceLoc, LEVEL_INFO, std::move(fmt), std::forward<Args>(args)...);
        }

        template <typename ...Args>
        void warn (const std::source_location sourceLoc, std::format_string<Args...> fmt, Args&&... args) {
            log(sourceLoc, LEVEL_WARN, std::move(fmt), std::forward<Args>(args)...);
        }

        template <typename ...Args>
        void error (const std::source_location sourceLoc, std::format_string<Args...> fmt, Args&&... args) {
            log(sourceLoc, LEVEL_ERROR, std::move(fmt), std::forward<Args>(args)...);
        }

        template <typename ...Args>
        void fatal (const std::source_location sourceLoc, std::format_string<Args...> fmt, Args&&... args) {
            log(sourceLoc, LEVEL_FATAL, std::move(fmt), std::forward<Args>(args)...);
        }
    };
}
