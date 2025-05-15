#ifndef NDEBUG
    #include <cpptrace/cpptrace.hpp>
#endif

#include "log_manager.h"
#include "logging/logger.h"
#include "logging/logging.h"

using namespace phenyl::logging;

void Logger::initSink () {
    if (m_sink) {
        return;
    }

    m_sink = LOG_MANAGER.getSink(this, m_parent);
}

Logger::Logger (const std::string_view name) : m_name{name}, m_parent{nullptr} {}

Logger::Logger (const std::string_view name, Logger& parent) : m_name{name}, m_parent{&parent} {}

void Logger::setMinLevel (const int level) {
    m_sink->setMinLogLevel(level);
}

void phenyl::PrintStackTrace () {
#ifndef NDEBUG
    cpptrace::generate_trace(1).print();
#else
#endif
}
