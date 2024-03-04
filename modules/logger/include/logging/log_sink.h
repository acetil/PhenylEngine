#pragma once

#include <source_location>
#include <string>
#include <vector>

#include "forward.h"

namespace phenyl::logging {
    class LogSink {
    private:
        std::string sinkName;
        std::string sinkPath;

        std::vector<LogSink*> childSinks;

        int minLogLevel = LEVEL_DEBUG;

    protected:
        virtual void log (const std::string& prefix, const std::string& logText) = 0;

    public:
        LogSink (std::string path);
        virtual ~LogSink() = default;

        void log (std::source_location sourceLoc, int level, const std::string& logText);

        [[nodiscard]] const std::string& getPath () const {
            return sinkPath;
        }

        int getMinLogLevel () const {
            return minLogLevel;
        }
        void setMinLogLevel (int logLevel, bool propagate=true);

        void setName (std::string name);

        void addChild (LogSink* childSink);
    };
}
