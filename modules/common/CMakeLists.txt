#add_library(common)
add_library(core include/core/maths/2d/transform.h src/common/maths/2d/transform.cpp include/core/components/2d/global_transform.h src/common/debug.cpp
        include/core/assets/assets.h
        include/core/assets/asset_manager.h
        include/core/assets/asset.h
        include/core/assets/forward.h
        src/common/assets/assets.cpp
        include/core/components/timed_lifetime.h
        src/common/components/timed_lifetime.cpp
        include/core/detail/loggers.h
        src/common/loggers.cpp
        include/core/plugins/timed_lifetime_plugin.h
        src/common/plugins/timed_lifetime_plugin.cpp
        include/core/plugins/core_plugin_2d.h
        src/common/plugins/core_plugin_2d.cpp
        include/core/input/input_device.h
        include/core/input/input_action.h
        src/common/input/input_action.cpp
        include/core/input/game_input.h
        src/common/input/game_input.cpp
        include/core/plugins/input_plugin.h
        src/common/plugins/input_plugin.cpp
        include/core/input/axis_action.h
        src/common/input/axis_input.cpp
        include/core/serialization/serializer.h
        src/common/serialization/serializer.cpp
        include/core/serialization/serializer_impl.h
        src/common/components/global_transform.cpp
        include/core/serialization/serializer_forward.h
        include/core/serialization/backends.h
        src/common/serialization/json_backend.cpp
        src/component/detail/component_vector.cpp
        src/component/detail/entity_id_list.cpp
        src/component/archetype.cpp
        src/component/children_view.cpp
        src/component/component.cpp
        src/component/entity.cpp
        src/component/prefab.cpp
        src/component/prefab_asset_manager.cpp
        src/component/query.cpp
        src/runtime/runtime.cpp
        src/runtime/stages.cpp
)

find_package(nlohmann_json REQUIRED)

set_property(TARGET core PROPERTY CXX_STANDARD 20)

target_include_directories(core PUBLIC include)
#target_include_directories(common INTERFACE include)
target_include_directories(core PRIVATE src)

#target_link_libraries(common PRIVATE logger maths eventbus)
#target_link_libraries(common PUBLIC util)
target_link_libraries(core PUBLIC maths util)
target_link_libraries(core PRIVATE logger graphics nlohmann_json::nlohmann_json)