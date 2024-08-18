add_library(component include/component/forward.h src/component/detail/entity_id_list.cpp src/component/detail/entity_id_list.cpp
        include/component/entity_id.h
        include/component/detail/entity_id_list.h
        include/component/detail/relationships.h
        include/component/component_serializer.h
        include/component/prefab_asset_manager.h
        src/component/prefab_asset_manager.cpp
        include/component/detail/loggers.h
        src/component/detail/loggers.cpp
        include/component/component.h
        include/component/detail/component_vector.h
        src/component/detail/component_vector.cpp
        include/component/archetype.h
        src/component/archetype.cpp
        include/component/detail/iarchetype_manager.h
        src/component/component.cpp
        include/component/entity.h
        src/component/entity.cpp
        include/component/query.h
        src/component/query.cpp
        include/component/archetype_view.h
        include/component/children_view.h
        src/component/children_view.cpp
        include/component/detail/component_instance.h
        include/component/detail/signal_handler.h
        include/component/prefab.h
        include/component/detail/prefab_factory.h
        src/component/prefab.cpp
        include/component/detail/archetype_key.h
)

set_property(TARGET component PROPERTY CXX_STANDARD 20)

target_include_directories(component PUBLIC include)
target_include_directories(component PRIVATE src)

#target_link_libraries(component PRIVATE logger)
target_link_libraries(component PUBLIC util common logger)