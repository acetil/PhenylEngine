add_library(logger OBJECT
        include/logging/logger.h
        include/logging/logging.h
        src/logger.cpp
        include/logging/log_sink.h
        src/log_sink.cpp
        src/log_manager.h
        src/log_manager.cpp
        src/stream_sink.cpp
        src/stream_sink.h
        include/logging/forward.h
        include/logging/properties.h
        src/properties.cpp)

find_package(cpptrace REQUIRED)

set_property(TARGET logger PROPERTY CXX_STANDARD 20)

target_include_directories(logger PUBLIC include)
target_include_directories(logger PRIVATE src)

target_link_libraries(logger PRIVATE cpptrace::cpptrace)