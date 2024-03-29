#pragma once

#include <logging/log_sink.h>

namespace phenyl::logging {
    class StreamSink : public LogSink {
    private:
        std::ostream& file;
    public:
        StreamSink (std::ostream& file, std::string path);

        void log(const std::string& prefix, const std::string& logText) override;
    };
}