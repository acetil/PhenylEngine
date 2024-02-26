#pragma once

#include <cstdlib>
#include <source_location>
#include <string>

#include "forward.h"


#ifndef PHENYL_MIN_LOG_LEVEL
#ifndef NDEBUG
#define PHENYL_MIN_LOG_LEVEL LEVEL_DEBUG
#else
#definePHENYL_MIN_LOG_LEVEL LEVEL_INFO
#endif
#endif

static_assert(PHENYL_MIN_LOG_LEVEL <= LEVEL_FATAL);

#if LEVEL_DEBUG >= PHENYL_MIN_LOG_LEVEL
#define PHENYL_LOGD(logger, fmt, ...) logger.debug(std::source_location::current(), fmt, __VA_ARGS__)

#define PHENYL_LOGD_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGD(logger, fmt, ...)
#else
#define PHENYL_LOGD(logger, fmt, ...)
#define PHENYL_LOGD_IF(cond, logger, fmt, ...)
#endif

#if LEVEL_INFO >= PHENYL_MIN_LOG_LEVEL
#define PHENYL_LOGI(logger, fmt, ...) logger.info(std::source_location::current(), fmt, __VA_ARGS__)
#define PHENYL_LOGI_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGI(logger, fmt, ...)
#else
#define PHENYL_LOGI(logger, fmt, ...)
#define PHENYL_LOGI_IF(cond, logger, fmt, ...)
#endif

#if LEVEL_WARN >= PHENYL_MIN_LOG_LEVEL
#define PHENYL_LOGW(logger, fmt, ...) logger.warn(std::source_location::current(), fmt, __VA_ARGS__)
#define PHENYL_LOGW_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGW(logger, fmt, ...)
#else
#define PHENYL_LOGW(logger, fmt, ...)
#define PHENYL_LOGW_IF(cond, logger, fmt, ...)
#endif

#if LEVEL_ERROR >= PHENYL_MIN_LOG_LEVEL
#define PHENYL_LOGE(logger, fmt, ...) logger.error(std::source_location::current(), fmt, __VA_ARGS__)
#define PHENYL_LOGE_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGE(logger, fmt, ...)
#else
#define PHENYL_LOGE(logger, fmt, ...)
#define PHENYL_LOGE_IF(cond, logger, fmt, ...)
#endif

#define PHENYL_LOGF(logger, fmt, ...) do { \
    logger.fatal(std::source_location::current(), fmt, __VA_ARGS__); \
    std::exit(1); \
} while (0)
#define PHENYL_LOGF_IF(cond, logger, fmt, ...) if (cond) PHENYL_LOGF(logger, fmt, ...)

#define PHENYL_ASSERT(logger, cond) do {\
    if (!(cond)) { \
        auto sourceLoc = std::source_location::current(); \
        logger.fatal(sourceLoc, "{}({}:{}): assertion failed: {}", sourceLoc.file_name(), sourceLoc.line(), sourceLoc.column(), #cond); \
        std::exit(1); \
    } \
} while (0)

#ifndef NDEBUG
#define PHENYL_DLOGI(logger, fmt, ...) PHENYL_LOGI(logger, fmt, ...)
#define PHENYL_DLOGI_IF(cond, logger, fmt, ...) PHENYL_LOGI_IF(cond, logger, fmt, ...)
#define PHENYL_DLOGW(logger, fmt, ...) PHENYL_LOGW(logger, fmt, ...)
#define PHENYL_DLOGW_IF(cond, logger, fmt, ...) PHENYL_LOGW_IF(cond, logger, fmt, ...)
#define PHENYL_DLOGE(logger, fmt, ...) PHENYL_LOGE(logger, fmt, ...)
#define PHENYL_DLOGE_IF(cond, logger, fmt, ...) PHENYL_LOGE_IF(cond, logger, fmt, ...)
#define PHENYL_DLOGF(logger, fmt, ...) PHENYL_LOGF(logger, fmt, ...)
#define PHENYL_DLOGF_IF(cond, logger, fmt, ...) PHENYL_LOGF_IF(cond, logger, fmt, ...)

#define PHENYL_DASSERT(logger, cond) PHENYL_ASSERT(logger, cond)
#else
#define PHENYL_DLOGI(logger, fmt, ...)
#define PHENYL_DLOGI_IF(cond, logger, fmt, ...)
#define PHENYL_DLOGW(logger, fmt, ...)
#define PHENYL_DLOGW_IF(cond, logger, fmt, ...)
#define PHENYL_DLOGE(logger, fmt, ...)
#define PHENYL_DLOGE_IF(cond, logger, fmt, ...)
#define PHENYL_DLOGF(logger, fmt, ...)
#define PHENYL_DLOGF_IF(cond, logger, fmt, ...)

#define PHENYL_DASSERT(logger, cond)
#endif

namespace phenyl {
    void InitLogging (int rootLevel, const std::string& logPath);
    void ShutdownLogging ();

    void SetLogLevel (const std::string& logger, int level);
}

