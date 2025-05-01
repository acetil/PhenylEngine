#pragma once

#include <logging/log_sink.h>

namespace phenyl::logging {
    class StreamSink : public LogSink {
    public:
        StreamSink (std::ostream& file, std::string path);

        void log(const std::string& prefix, const std::string& logText) override;

    private:
        std::ostream& m_file;
    };
}