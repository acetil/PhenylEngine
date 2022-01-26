add_library(logger src/logging.cpp src/logging_internal.cpp src/format.cpp)

set_property(TARGET logger PROPERTY CXX_STANDARD 20)

target_include_directories(logger PUBLIC include)
target_include_directories(logger PRIVATE src)

