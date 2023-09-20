#include "logging/logging.h"
#include "logging_internal.h"
#include "stdarg.h"

#define COMPONENT_LOCATION "COMPONENT"
#define EVENT_LOCATION "EVENT"
#define GAME_LOCATION "GAME"
#define GRAPHICS_LOCATION "GRAPHICS"
#define PHYSICS_LOCATION "PHYSICS"
#define UTIL_LOCATION "UTIL"
#define COMMON_LOCATION "COMMON"
#define SERIALIZER_LOCATION "SERIALIZER"

using namespace phenyl;

/*void component::logging::log (int level, const char* log) {
    internal::log_internal(level, COMPONENT_LOCATION, log);
}*/
void component::logging::log (int level, const std::string& log) {
    internal::log_internal(level, COMPONENT_LOCATION, log);
}
/*void component::logging::logf (int level, const char* log, ...) {
    va_list l;
    va_start(l, log);
    internal::vlogf_internal(level, COMPONENT_LOCATION, log, l);
    va_end(l);
}*/

/*void event::logging::log (int level, const char* log) {
    internal::log_internal(level, EVENT_LOCATION, log);
}*/
void event::logging::log (int level, const std::string& log) {
    internal::log_internal(level, EVENT_LOCATION, log);
}
/*void event::logging::logf (int level, const char* log, ...) {
    va_list l;
    va_start(l, log);
    internal::vlogf_internal(level, EVENT_LOCATION, log, l);
    va_end(l);
}*/

/*void game::logging::log (int level, const char* log) {
    internal::log_internal(level, GAME_LOCATION, log);
}*/
void game::logging::log (int level, const std::string& log) {
    internal::log_internal(level, GAME_LOCATION, log);
}
/*void game::logging::logf (int level, const char* log, ...) {
    va_list l;
    va_start(l, log);
    internal::vlogf_internal(level, GAME_LOCATION, log, l);
    va_end(l);
}*/
/*void graphics::logging::log (int level, const char* log) {
    internal::log_internal(level, GRAPHICS_LOCATION, log);
}*/
void graphics::logging::log (int level, const std::string& log) {
    internal::log_internal(level, GRAPHICS_LOCATION, log);
}
/*void graphics::logging::logf (int level, const char* log, ...) {
    va_list l;
    va_start(l, log);
    internal::vlogf_internal(level, GRAPHICS_LOCATION, log, l);
    va_end(l);
}*/
/*void physics::logging::log (int level, const char* log) {
    internal::log_internal(level, PHYSICS_LOCATION, log);
}*/
/*void physics::logging::logf (int level, const char* log, ...) {
    va_list l;
    va_start(l, log);
    internal::vlogf_internal(level, PHYSICS_LOCATION, log, l);
    va_end(l);
}*/

void physics::logging::log (int level, const std::string& log) {
    internal::log_internal(level, PHYSICS_LOCATION, log);
}

void util::logging::log(int level, const std::string& log) {
    internal::log_internal(level, UTIL_LOCATION, log);
}

void common::logging::log (int level, const std::string& log) {
    internal::log_internal(level, COMMON_LOCATION, log);
}

void serializer::logging::log (int level, const std::string& log) {
    internal::log_internal(level, SERIALIZER_LOCATION, log);
}

void logger::initLogger (const std::string& logfile) {
    internal::init_internal(logfile);
}

void logger::initLogger () {
    initLogger("debug.log");
}

void logger::log (int level, const std::string& category, const std::string& log) {
    internal::log_internal(level, category, log);
}