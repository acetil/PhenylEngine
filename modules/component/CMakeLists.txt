add_library(component include/component/main_component.h src/component/component_extern.cpp src/component/rotation_component.cpp include/component/position.h src/component/position.cpp src/component/view/game_view.cpp include/component/component_serialiser.h include/component/forward.h)

set_property(TARGET component PROPERTY CXX_STANDARD 20)

target_include_directories(component PUBLIC include)
target_include_directories(component PRIVATE src)

target_link_libraries(component PRIVATE logger maths engine eventbus)
target_link_libraries(component PUBLIC util)