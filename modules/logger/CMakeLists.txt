add_library(logger src/logging.cpp src/logging_internal.cpp)

target_include_directories(logger PUBLIC include)
target_include_directories(logger PRIVATE src)

