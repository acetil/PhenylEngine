#pragma once

#include <format>
#include <string>

#include "log_sink.h"

namespace phenyl::logging {
    class Logger {
    private:
        std::string_view name;
        Logger* parent;
        LogSink* logSink = nullptr;

        void initSink ();

        void setMinLevel (int level);

        friend class LogManager;
    public:
        explicit Logger (std::string_view name);
        Logger (std::string_view name, Logger& parent);

        template <typename ...Args>
        void log (const std::source_location sourceLoc, const int level, std::format_string<Args...> fmt, Args&&... args) {
            if (!logSink) {
                [[unlikely]]
                initSink();
            }

            if (level < logSink->getMinLogLevel()) {
                return;
            }

            logSink->log(sourceLoc, level, std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename ...Args>
        void trace (const std::source_location sourceLoc, std::format_string<Args...> fmt, Args&&... args) {
            log(sourceLoc, LEVEL_TRACE, std::move(fmt), std::forward<Args>(args)...);
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
