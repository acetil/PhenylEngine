#add_library(common)
add_library(common include/common include/common/input/input_source.h include/common/input/remappable_input.h src/common/input/remappable_input.cpp include/common/events/debug/reload_theme.h include/common/events/theme_change.h include/common/scope.h include/common/input/proxy_source.h src/common/input/proxy_source.cpp include/common/input/forward.h include/common/input/remappable_proxy_input.h src/common/input/remappable_proxy_input.cpp include/common/maths/2d/transform.h src/common/maths/2d/transform.cpp include/common/components/2d/global_transform.h src/common/components/2d/global_transform.cpp include/common/debug.h src/common/debug.cpp
        include/common/events/debug/debug_render.h
        include/common/events/debug/debug_pause.h
        include/common/events/debug/debug_step.h
        include/common/serializer.h
        include/common/maths/glm_serializer.h
        include/common/maths/2d/transform_serializer.h
        include/common/components/2d/global_transform_serialize.h
        include/common/serializer_intrusive.h
        include/common/assets/assets.h
        include/common/assets/asset_manager.h
        include/common/assets/asset.h
        include/common/assets/forward.h
        src/common/assets/assets.cpp
)

find_package(nlohmann_json REQUIRED)

set_property(TARGET common PROPERTY CXX_STANDARD 20)

target_include_directories(common PUBLIC include)
#target_include_directories(common INTERFACE include)
target_include_directories(common PRIVATE src)

#target_link_libraries(common PRIVATE logger maths eventbus)
#target_link_libraries(common PUBLIC util)
target_link_libraries(common PUBLIC maths eventbus util nlohmann_json::nlohmann_json)
target_link_libraries(common PRIVATE logger graphics)