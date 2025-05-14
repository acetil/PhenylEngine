add_library(util OBJECT src/profiler.cpp src/string_help.cpp include/util/fl_vector.h include/util/iterable.h
        include/util/random.h
        src/random.cpp
        include/util/endian.h
        include/util/istream_help.h
        include/util/detail/loggers.h
        src/loggers.cpp
        include/util/hash.h
        include/util/range_utils.h)

find_package(nlohmann_json REQUIRED)
find_package(cpptrace REQUIRED)

set_property(TARGET util PROPERTY CXX_STANDARD 20)

target_include_directories(util PUBLIC include)
target_include_directories(util PRIVATE src)

target_link_libraries(util PRIVATE logger nlohmann_json::nlohmann_json)
target_link_libraries(util PUBLIC cpptrace::cpptrace)
