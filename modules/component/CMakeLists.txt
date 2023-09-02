add_library(component src/component/view/game_view.cpp include/component/component_serialiser.h include/component/forward.h include/component/fixed_component.h include/component/component.h src/component/detail/entity_id_list.cpp src/component/detail/entity_id_list.cpp
        include/component/entity_id.h
        include/component/detail/component_set.h
        include/component/detail/entity_id_list.h
        src/component/detail/component_set.cpp
        include/component/detail/signal_handler.h
        include/component/detail/utils.h
        include/component/signals/component_update.h
        include/component/detail/component_signal_handler.h)

set_property(TARGET component PROPERTY CXX_STANDARD 20)

target_include_directories(component PUBLIC include)
target_include_directories(component PRIVATE src)

target_link_libraries(component PRIVATE logger maths engine eventbus)
target_link_libraries(component PUBLIC util)