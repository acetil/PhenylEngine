#include "stream_sink.h"

#include <iostream>

using namespace phenyl::logging;

StreamSink::StreamSink (std::ostream& file, std::string path) : LogSink{std::move(path)}, m_file{file} {}

void StreamSink::log (const std::string& prefix, const std::string& logText) {
    std::cout << prefix << ": " << logText << "\n";
    m_file << prefix << ": " << logText << "\n";
}
