add_library(component include/component/forward.h include/component/fixed_component.h include/component/component.h src/component/detail/entity_id_list.cpp src/component/detail/entity_id_list.cpp
        include/component/entity_id.h
        include/component/detail/component_set.h
        include/component/detail/entity_id_list.h
        src/component/detail/component_set.cpp
        include/component/detail/signals/signal_handler.h
        include/component/detail/component_utils.h
        include/component/signals/component_update.h
        include/component/detail/signals/component_handler.h
        include/component/prefab.h
        include/component/detail/prefab_list.h
        src/component/detail/prefab_list.cpp
        src/component/component.cpp
        include/component/detail/managers/basic_manager.h
        include/component/entity.h
        include/component/detail/children_view.h
        src/component/entity_view.cpp
        src/component/detail/basic_manager.cpp
        include/component/detail/relationships.h
        include/component/query.h
        include/component/detail/managers/query_manager.h
        include/component/detail/signals/handler_list.h
        include/component/detail/typed_set.h
        include/component/detail/signals/component_handler_base.h
        include/component/signals/children_update.h
        include/component/component_serializer.h
        include/component/prefab_manager.h
        src/component/prefab_manager.cpp
        include/component/detail/loggers.h
        src/component/detail/loggers.cpp
        include/component2/component.h
        include/component2/detail/component_vector.h
        src/component2/detail/component_vector.cpp
        include/component2/archetype.h
        src/component2/archetype.cpp
        include/component2/detail/iarchetype_manager.h
        src/component2/component.cpp
        include/component2/entity2.h
        src/component2/entity2.cpp
)

set_property(TARGET component PROPERTY CXX_STANDARD 20)

target_include_directories(component PUBLIC include)
target_include_directories(component PRIVATE src)

#target_link_libraries(component PRIVATE logger)
target_link_libraries(component PUBLIC util common logger)