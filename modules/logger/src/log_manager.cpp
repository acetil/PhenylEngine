#include <logging/logging2.h>

#include "log_manager.h"

#include <ranges>

#include "stream_sink.h"

using namespace phenyl::logging;

LogManager phenyl::logging::LOG_MANAGER{};

static Logger LOGGER = Logger{"LOG_MANAGER"};

void LogManager::init (const int rootLogLevel, const std::string& logPath) {
    this->rootLogLevel = rootLogLevel;
    logFile = std::ofstream{logPath};
}

LogSink* LogManager::makeSink (Logger* logger, Logger* parent) {
    PHENYL_DASSERT(LOGGER, logFile);

    std::string path;
    int logLevel;
    if (parent) {
        parent->initSink();

        path = parent->logSink->getPath() + "." + logger->name;
        logLevel = parent->minLogLevel;
    } else {
        path = logger->name;
        logLevel = rootLogLevel;
    }

    PHENYL_ASSERT(LOGGER, !sinks.contains(path));

    if (const auto levelIt = logLevels.find(path); levelIt != logLevels.end()) {
        logLevel = levelIt->second;
    }

    logger->setMinLevel(logLevel);

    auto sink = std::make_unique<StreamSink>(logFile, logger->name, std::move(path));
    auto* ptr = sink.get();

    sinks.emplace(ptr->getPath(), std::move(sink));
    loggers.emplace(ptr->getPath(), logger);

    return ptr;
}

void LogManager::setLogLevel (const std::string& loggerPath, const int level) {
    if (loggerPath.empty()) {
        rootLogLevel = level;
        return;
    }

    if (const auto it = loggers.find(loggerPath); it != loggers.end()) {
        it->second->setMinLevel(level);
    }

    logLevels.emplace(loggerPath, level);

    // TODO: child log levels
}

void LogManager::shutdownLogging () {
    for (auto* logger : std::ranges::views::values(loggers)) {
        logger->logSink = nullptr;
    }

    loggers.clear();
    logLevels.clear();
    sinks.clear();

    logFile.flush();
    logFile = std::ofstream{};
}

void phenyl::InitLogging (const int rootLevel, const std::string& logPath) {
    LOG_MANAGER.init(rootLevel, logPath);
}

void phenyl::SetLogLevel (const std::string& logger, const int level) {
    LOG_MANAGER.setLogLevel(logger, level);
}

void phenyl::ShutdownLogging () {
    LOG_MANAGER.shutdownLogging();
}