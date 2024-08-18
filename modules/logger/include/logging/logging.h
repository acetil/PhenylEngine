#pragma once

#include <cstdlib>
#include <exception>
#include <source_location>
#include <string>

#ifndef NDEBUG
#include <cpptrace/cpptrace.hpp>
#endif

#include "logger.h"


namespace phenyl {
    using Logger = logging::Logger;
    extern Logger PHENYL_LOGGER;
}

#ifndef PHENYL_MIN_LOG_LEVEL
#ifndef NDEBUG
#define PHENYL_MIN_LOG_LEVEL LEVEL_TRACE
#else
#define PHENYL_MIN_LOG_LEVEL LEVEL_INFO
#endif
#endif

#ifndef PHENYL_DEFAULT_ROOT_LEVEL
#ifndef NDEBUG
#define PHENYL_DEFAULT_ROOT_LEVEL LEVEL_DEBUG
#else
#define PHENYL_DEFAULT_ROOT_LEVEL LEVEL_INFO
#endif
#endif

static_assert(PHENYL_MIN_LOG_LEVEL <= LEVEL_FATAL);

#ifndef NDEBUG
#define PRINT_STACKTRACE() cpptrace::generate_trace().print()
#else
#define PRINT_STACKTRACE()
#endif

#if LEVEL_DEBUG >= PHENYL_MIN_LOG_LEVEL
#define PHENYL_LOGD(logger, fmt, ...) logger.debug(std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_LOGD_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGD(logger, fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define PHENYL_LOGD(logger, fmt, ...)
#define PHENYL_LOGD_IF(cond, logger, fmt, ...)
#endif

#if LEVEL_INFO >= PHENYL_MIN_LOG_LEVEL
#define PHENYL_LOGI(logger, fmt, ...) logger.info(std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_LOGI_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGI(logger, fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define PHENYL_LOGI(logger, fmt, ...)
#define PHENYL_LOGI_IF(cond, logger, fmt, ...)
#endif

#if LEVEL_WARN >= PHENYL_MIN_LOG_LEVEL
#define PHENYL_LOGW(logger, fmt, ...) logger.warn(std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_LOGW_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGW(logger, fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define PHENYL_LOGW(logger, fmt, ...)
#define PHENYL_LOGW_IF(cond, logger, fmt, ...)
#endif

#if LEVEL_ERROR >= PHENYL_MIN_LOG_LEVEL
#define PHENYL_LOGE(logger, fmt, ...) logger.error(std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_LOGE_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGE(logger, fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define PHENYL_LOGE(logger, fmt, ...)
#define PHENYL_LOGE_IF(cond, logger, fmt, ...)
#endif

#define PHENYL_LOGF(logger, fmt, ...) do { \
    logger.fatal(std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__); \
PRINT_STACKTRACE(); \
} while (0)
#define PHENYL_LOGF_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGF(logger, fmt __VA_OPT__(,) __VA_ARGS__)

#define PHENYL_ASSERT(cond) do {\
    if (!(cond)) { \
        auto sourceLoc = std::source_location::current(); \
        ::phenyl::PHENYL_LOGGER.fatal(sourceLoc, "{}({}:{}): assertion failed: {}", sourceLoc.file_name(), sourceLoc.line(), sourceLoc.column(), #cond); \
        PRINT_STACKTRACE(); \
        std::terminate(); \
    } \
} while (0)

#define PHENYL_ASSERT_MSG(cond, fmt, ...) do {\
if (!(cond)) { \
auto sourceLoc = std::source_location::current(); \
::phenyl::PHENYL_LOGGER.fatal(sourceLoc, "{}({}:{}): assertion failed: {}: {}", sourceLoc.file_name(), sourceLoc.line(), sourceLoc.column(), #cond, std::format(fmt __VA_OPT__(,) __VA_ARGS__)); \
PRINT_STACKTRACE(); \
std::terminate(); \
} \
} while (0)

#define PHENYL_ABORT(fmt, ...) do { \
auto sourceLoc = std::source_location::current(); \
::phenyl::PHENYL_LOGGER.fatal(sourceLoc, "{}({}:{}): runtime abort: {}", sourceLoc.file_name(), sourceLoc.line(), sourceLoc.column(), std::format(fmt __VA_OPT__(,) __VA_ARGS__)); \
PRINT_STACKTRACE(); \
std::terminate();\
} while (0)

#ifndef NDEBUG
#if LEVEL_TRACE >= PHENYL_MIN_LOG_LEVEL
#define PHENYL_TRACE(logger, fmt, ...) logger.trace(std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_TRACE_IF(cond, logger, fmt, ...) if (cond) PHENYL_TRACE(logger, fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define PHENYL_TRACE(logger, fmt, ...)
#define PHENYL_TRACE_IF(cond, logger, fmt, ...)
#endif

#define PHENYL_DLOGI(logger, fmt, ...) PHENYL_LOGI(logger, fmt __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_DLOGI_IF(cond, logger, fmt, ...) PHENYL_LOGI_IF(cond, logger, fmt__VA_OPT__(,) __VA_ARGS__)
#define PHENYL_DLOGW(logger, fmt, ...) PHENYL_LOGW(logger, fmt __VA_OPT__(,) __VA_ARGS__.)
#define PHENYL_DLOGW_IF(cond, logger, fmt, ...) PHENYL_LOGW_IF(cond, logger, fmt__VA_OPT__(,) __VA_ARGS__)
#define PHENYL_DLOGE(logger, fmt, ...) PHENYL_LOGE(logger, fmt __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_DLOGE_IF(cond, logger, fmt, ...) PHENYL_LOGE_IF(cond, logger, fmt __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_DLOGF(logger, fmt, ...) PHENYL_LOGF(logger, fmt __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_DLOGF_IF(cond, logger, fmt, ...) PHENYL_LOGF_IF(cond, logger, fmt __VA_OPT__(,) __VA_ARGS__)

#define PHENYL_DASSERT(cond) PHENYL_ASSERT(cond)
#define PHENYL_DASSERT_MSG(cond, fmt, ...) PHENYL_ASSERT_MSG(cond, fmt __VA_OPT__(,) __VA_ARGS__)

#define PHENYL_DEBUG(...) __VA_ARGS__
#else
#define PHENYL_TRACE
#define PHENYL_TRACE_IF

#define PHENYL_DLOGI(logger, fmt, ...)
#define PHENYL_DLOGI_IF(cond, logger, fmt, ...)
#define PHENYL_DLOGW(logger, fmt, ...)
#define PHENYL_DLOGW_IF(cond, logger, fmt, ...)
#define PHENYL_DLOGE(logger, fmt, ...)
#define PHENYL_DLOGE_IF(cond, logger, fmt, ...)
#define PHENYL_DLOGF(logger, fmt, ...)
#define PHENYL_DLOGF_IF(cond, logger, fmt, ...)

#define PHENYL_DASSERT(cond)
#define PHENYL_DASSERT_MSG(cond, msg, ...)

#define PHENYL_DEBUG(...)
#endif

namespace phenyl {
    void InitLogging (const logging::LoggingProperties& properties);
    void ShutdownLogging ();

    void SetLogLevel (const std::string& logger, int level);
}

