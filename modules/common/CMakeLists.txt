#add_library(common)
add_library(common include/common include/common/scope.h include/common/maths/2d/transform.h src/common/maths/2d/transform.cpp include/common/components/2d/global_transform.h include/common/debug.h src/common/debug.cpp
        include/common/assets/assets.h
        include/common/assets/asset_manager.h
        include/common/assets/asset.h
        include/common/assets/forward.h
        src/common/assets/assets.cpp
        include/common/components/timed_lifetime.h
        src/common/components/timed_lifetime.cpp
        include/common/detail/loggers.h
        src/common/loggers.cpp
        include/common/plugins/timed_lifetime_plugin.h
        src/common/plugins/timed_lifetime_plugin.cpp
        include/common/plugins/core_plugin_2d.h
        src/common/plugins/core_plugin_2d.cpp
        include/common/input/input_device.h
        include/common/input/input_action.h
        src/common/input/input_action.cpp
        include/common/input/game_input.h
        src/common/input/game_input.cpp
        include/common/plugins/input_plugin.h
        src/common/plugins/input_plugin.cpp
        include/common/input/axis_action.h
        src/common/input/axis_input.cpp
        include/common/serialization/serializer.h
        src/common/serialization/serializer.cpp
        include/common/serialization/serializer_impl.h
        src/common/components/global_transform.cpp
        include/common/serialization/serializer_forward.h
        include/common/serialization/backends.h
        src/common/serialization/json_backend.cpp
)

find_package(nlohmann_json REQUIRED)

set_property(TARGET common PROPERTY CXX_STANDARD 20)

target_include_directories(common PUBLIC include)
#target_include_directories(common INTERFACE include)
target_include_directories(common PRIVATE src)

#target_link_libraries(common PRIVATE logger maths eventbus)
#target_link_libraries(common PUBLIC util)
target_link_libraries(common PUBLIC maths util runtime nlohmann_json::nlohmann_json)
target_link_libraries(common PRIVATE logger graphics component)