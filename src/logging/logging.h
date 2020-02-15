#ifndef LOGGING_H
#define LOGGING_H

#define LEVEL_INFO 0
#define LEVEL_WARNING 1
#define LEVEL_ERROR 2
#define LEVEL_FATAL 3
#define LEVEL_DEBUG 4

namespace component {
    namespace logging {
        void log (int level, const char* log);
        void logf (int level, const char* log, ...);
    };
};
namespace event {
    namespace logging {
        void log (int level, const char* log);
        void logf (int level, const char* log, ...);
    }
}
namespace game {
    namespace logging {
        void log (int level, const char* log);
        void logf (int level, const char* log, ...);
    }
}
namespace graphics {
    namespace logging {
        void log (int level, const char* log);
        void logf (int level, const char* log, ...);
    }
}
namespace physics {
    namespace logging {
        void log (int level, const char* log);
        void logf (int level, const char* log, ...);
    }
}
#endif