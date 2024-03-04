#include <iostream>

#include "stream_sink.h"


using namespace phenyl::logging;

StreamSink::StreamSink(std::ostream& file, std::string path) : LogSink{std::move(path)}, file{file} {}

void StreamSink::log(const std::string& prefix, const std::string& logText) {
    std::cout << prefix << ": " << logText << "\n";
    file << prefix << ": " << logText << "\n";
}
