#pragma once

#define LEVEL_TRACE (-1)
#define LEVEL_DEBUG 0
#define LEVEL_INFO 1
#define LEVEL_WARN 2
#define LEVEL_ERROR 3
#define LEVEL_FATAL 4

namespace phenyl::logging {
    class LogSink;
    class Logger;
    class LoggingProperties;
}