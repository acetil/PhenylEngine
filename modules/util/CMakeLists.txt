add_library(util src/data.cpp src/profiler.cpp src/string_help.cpp src/test_data.cpp include/util/optional.h include/util/detail/memory.h include/util/fixed_stack.h include/util/set.h src/set_test.cpp)

find_package(nlohmann_json 3.2.0 REQUIRED)

set_property(TARGET util PROPERTY CXX_STANDARD 20)

target_include_directories(util PUBLIC include)
target_include_directories(util PRIVATE src)

target_link_libraries(util PRIVATE logger)