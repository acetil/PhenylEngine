#include <string>
#include "util/format.h"
#ifndef LOGGING_H
#define LOGGING_H

#define LEVEL_INFO 0
#define LEVEL_WARNING 1
#define LEVEL_ERROR 2
#define LEVEL_FATAL 3
#define LEVEL_DEBUG 4

namespace component::logging {
    //void log (int level, const char* log);
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        //std::string str = util::format(logStr, args...);
        //log(level, str);
        log(level, util::format(logStr, args...));
    }
    //void logf (int level, const char* log, ...);
}
namespace event::logging {
    //void log (int level, const char* log);
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
    //void logf (int level, const char* log, ...);
}
namespace game::logging {
    //void log (int level, const char* log);
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
    //void logf (int level, const char* log, ...);
}
namespace graphics::logging {
    //void log (int level, const char* log);
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
    //void logf (int level, const char* log, ...);
}
namespace physics::logging {
    //void log (int level, const char* log);
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
    //void logf (int level, const char* log, ...);
}
namespace util::logging {
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
}

namespace logger {
    void initLogger (const std::string& logfile);
    void initLogger ();
}
#endif