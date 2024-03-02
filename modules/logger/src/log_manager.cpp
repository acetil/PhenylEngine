#include <logging/logging.h>

#include "log_manager.h"

#include <ranges>

#include "stream_sink.h"

using namespace phenyl::logging;

LogManager phenyl::logging::LOG_MANAGER{};

Logger phenyl::PHENYL_LOGGER{"PHENYL"};
static Logger LOGGER = Logger{"LOG_MANAGER"};

void LogManager::init (const int rootLogLevel, const std::string& logPath) {
    this->rootLogLevel = rootLogLevel;
    logFile = std::ofstream{logPath};
}

LogSink* LogManager::makeSink (Logger* logger, Logger* parent) {
    PHENYL_DASSERT(logFile);

    std::string path;
    int logLevel;
    if (parent) {
        parent->initSink();

        path = parent->logSink->getPath() + "." + std::string{logger->name};
        logLevel = parent->minLogLevel;
    } else {
        path = logger->name;
        logLevel = rootLogLevel;
    }

    PHENYL_ASSERT(!sinks.contains(path));

    if (const auto levelIt = logLevels.find(path); levelIt != logLevels.end()) {
        logLevel = levelIt->second;
    }

    logger->setMinLevel(logLevel);

    auto sink = std::make_unique<StreamSink>(logFile, std::string{logger->name}, std::move(path));
    auto* ptr = sink.get();

    sinks.emplace(ptr->getPath(), std::move(sink));
    loggers.emplace(ptr->getPath(), logger);

    return ptr;
}

void LogManager::setLogLevel (const std::string& loggerPath, const int level) {
    if (loggerPath.empty()) {
        PHENYL_TRACE(LOGGER, "Setting root log level: level={}", level);
        rootLogLevel = level;
        return;
    }

    PHENYL_TRACE(LOGGER, "Setting log level of {}: level={}", loggerPath, level);
    if (const auto it = loggers.find(loggerPath); it != loggers.end()) {
        it->second->setMinLevel(level);
    }

    logLevels.emplace(loggerPath, level);

    // TODO: child log levels
}

void LogManager::shutdownLogging () {
    for (auto* logger : std::ranges::views::values(loggers)) {
        PHENYL_TRACE(LOGGER, "Clearing sink for {}", logger->logSink->getPath());
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
    PHENYL_LOGI(LOGGER, "Initialised logging to {}", logPath);
}

void phenyl::SetLogLevel (const std::string& logger, const int level) {
    LOG_MANAGER.setLogLevel(logger, level);
}

void phenyl::ShutdownLogging () {
    PHENYL_LOGI(LOGGER, "Shutting down logging");
    LOG_MANAGER.shutdownLogging();
}