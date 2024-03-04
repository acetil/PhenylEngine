#include <logging/logging.h>

#include "log_manager.h"

#include <algorithm>
#include <ranges>

#include "stream_sink.h"
#include "logging/properties.h"

using namespace phenyl::logging;

LogManager phenyl::logging::LOG_MANAGER{};

Logger phenyl::PHENYL_LOGGER{"PHENYL"};
static Logger LOGGER = Logger{"LOG_MANAGER"};

static std::string ToLower (std::string str) {
    std::ranges::transform(str, str.begin(), [] (const auto c) { return std::tolower(c); });
    return str;
}

void LogManager::init (const int rootLogLevel, const std::string& logPath) {
    this->rootLogLevel = rootLogLevel;
    logFile = std::ofstream{logPath};
    PHENYL_LOGI(LOGGER, "Initialised logging to {} (default level: {})", logPath, rootLogLevel);
}

LogSink* LogManager::getSink (const Logger* logger, Logger* parent) {
    PHENYL_DASSERT(logFile);

    std::string path;
    if (parent) {
        parent->initSink();

        path = ToLower(parent->logSink->getPath() + "." + std::string{logger->name});
    } else {
        path = ToLower(std::string{logger->name});
    }

    auto* sink = getSink(path);
    sink->setName(std::string{logger->name});
    return sink;
}

LogSink* LogManager::getSink (std::string_view path) {
    if (const auto sinkIt = sinks.find(path); sinkIt != sinks.end()) {
        return sinkIt->second.get();
    }

    auto pos = path.find_last_of('.');
    LogSink* parent = nullptr;

    if (pos != std::string_view::npos) {
        parent = getSink(path.substr(0, pos));
    }

    std::unique_ptr<LogSink> sink;
    if (parent) {
        sink = std::make_unique<StreamSink>(logFile, std::string{path});

        parent->addChild(sink.get());
        sink->setMinLogLevel(parent->getMinLogLevel());
    } else {
        sink = std::make_unique<StreamSink>(logFile, std::string{path});
        sink->setMinLogLevel(rootLogLevel);
    }

    auto* ptr = sink.get();
    sinks.emplace(std::string{path}, std::move(sink));

    return ptr;
}

void LogManager::setLogLevel (const std::string& loggerPath, int level, bool propagate) {
    if (loggerPath.empty()) {
        PHENYL_TRACE(LOGGER, "Setting root log level: level={}", level);
        rootLogLevel = level;

        if (propagate) {
            for (const auto& sink : std::ranges::views::values(sinks)) {
                sink->setMinLogLevel(level, false);
            }
        }

        return;
    }

    PHENYL_TRACE(LOGGER, "Setting log level of {}: level={}", loggerPath, level);
    getSink(loggerPath)->setMinLogLevel(level);
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

void phenyl::InitLogging (const LoggingProperties& properties) {
    LOG_MANAGER.init(properties.rootLogLevel, properties.logFile);

    for (const auto& [logger, level] : properties.logLevels) {
        LOG_MANAGER.setLogLevel(logger, level, false);
    }
}

void phenyl::SetLogLevel (const std::string& logger, const int level) {
    LOG_MANAGER.setLogLevel(logger, level);
}

void phenyl::ShutdownLogging () {
    PHENYL_LOGI(LOGGER, "Shutting down logging");
    LOG_MANAGER.shutdownLogging();
}