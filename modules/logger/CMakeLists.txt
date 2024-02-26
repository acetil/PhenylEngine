add_library(logger src/logging.cpp src/logging_internal.cpp src/format.cpp
        include/logging/logger.h
        src/logger.cpp
        include/logging/log_sink.h
        src/log_sink.cpp
        src/log_manager.h
        src/log_manager.cpp
        src/stream_sink.cpp
        src/stream_sink.h
        include/logging/logging2.h
        include/logging/forward.h)

set_property(TARGET logger PROPERTY CXX_STANDARD 20)

target_include_directories(logger PUBLIC include)
target_include_directories(logger PRIVATE src)

