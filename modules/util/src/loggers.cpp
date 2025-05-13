#include "util/detail/loggers.h"

using namespace phenyl;

Logger util::detail::UTIL_LOGGER{"UTIL", PHENYL_LOGGER};
Logger util::detail::OPTIONAL_LOGGER{"OPTIONAL", UTIL_LOGGER};
Logger util::detail::FL_VECTOR_LOGGER{"FL_VECTOR", UTIL_LOGGER};
