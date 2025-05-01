#pragma once

#include <source_location>
#include <string>
#include <vector>

#include "forward.h"

namespace phenyl::logging {
    class LogSink {
    public:
        LogSink (std::string path);
        virtual ~LogSink() = default;

        void log (std::source_location sourceLoc, int level, const std::string& logText);

        [[nodiscard]] const std::string& getPath () const {
            return m_path;
        }

        int getMinLogLevel () const {
            return m_minLogLevel;
        }
        void setMinLogLevel (int logLevel, bool propagate=true);

        void setName (std::string name);

        void addChild (LogSink* childSink);

    protected:
        virtual void log (const std::string& prefix, const std::string& logText) = 0;

    private:
        std::string m_name;
        std::string m_path;

        std::vector<LogSink*> m_children;

        int m_minLogLevel = LEVEL_DEBUG;

    };
}
