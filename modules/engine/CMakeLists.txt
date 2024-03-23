add_library(engine include src include/engine/engine.h src/engine/engine.cpp include/engine/input/game_input.h src/engine/input/game_input.cpp
        include/engine/level/level.h
        src/engine/level/level.cpp
        include/engine/level/level_manager.h
        include/engine/application.h
        include/engine/forward.h
        src/engine/application.cpp
        include/engine/plugins/level_plugin.h
        src/engine/plugins/level_plugin.cpp
        include/engine/plugins/prefab_plugin.h
        src/engine/plugins/prefab_plugin.cpp
        src/engine/plugins/app_plugin.h
        src/engine/plugins/app_plugin.cpp
        include/engine/plugins/default_2d_plugin.h
        src/engine/plugins/default_2d_plugin.cpp)

find_package(nlohmann_json REQUIRED)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)

target_include_directories(engine PUBLIC include)
target_include_directories(engine PRIVATE src)

target_link_libraries(engine PRIVATE logger component util maths graphics physics common nlohmann_json audio runtime)