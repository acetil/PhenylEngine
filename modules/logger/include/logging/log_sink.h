#pragma once

#include <source_location>
#include <string>

namespace phenyl::logging {
    class LogSink {
    private:
        std::string sinkName;
        std::string sinkPath;

    protected:
        virtual void log (const std::string& prefix, const std::string& logText) = 0;

    public:
        LogSink (std::string name, std::string path);
        virtual ~LogSink() = default;

        void log (std::source_location sourceLoc, int level, const std::string& logText);

        [[nodiscard]] const std::string& getPath () const {
            return sinkPath;
        }
    };
}