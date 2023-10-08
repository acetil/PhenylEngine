#pragma once

#include <string>
#include "util/format.h"

#define LEVEL_INFO 0
#define LEVEL_WARNING 1
#define LEVEL_ERROR 2
#define LEVEL_FATAL 3
#define LEVEL_DEBUG 4

namespace phenyl::component::logging {
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
namespace phenyl::event::logging {
    //void log (int level, const char* log);
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
    //void logf (int level, const char* log, ...);
}
namespace phenyl::game::logging {
    //void log (int level, const char* log);
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
    //void logf (int level, const char* log, ...);
}
namespace phenyl::graphics::logging {
    //void log (int level, const char* log);
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
    //void logf (int level, const char* log, ...);
}
namespace phenyl::physics::logging {
    //void log (int level, const char* log);
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
    //void logf (int level, const char* log, ...);
}
namespace phenyl::util::logging {
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
}

namespace phenyl::common::logging {
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
}

namespace phenyl::serializer::logging {
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
}

namespace phenyl::audio::logging {
    void log (int level, const std::string& log);
    template<typename ...Args>
    void log (int level, const std::string& logStr, Args... args) {
        log(level, util::format(logStr, args...));
    }
}

namespace phenyl::logger {
    void initLogger (const std::string& logfile);
    void initLogger ();

    void log (int level, const std::string& category, const std::string& log);
}
