add_library(engine include src
        include/engine/level/level.h
        src/engine/level/level.cpp
        include/engine/level/level_manager.h
        include/engine/forward.h)

find_package(nlohmann_json REQUIRED)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)

target_include_directories(engine PUBLIC include)
target_include_directories(engine PRIVATE src)

target_link_libraries(engine PRIVATE logger component util maths graphics physics common nlohmann_json audio runtime)