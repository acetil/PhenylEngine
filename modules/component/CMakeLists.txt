add_library(component src/component/view/game_view.cpp include/component/component_serialiser.h include/component/forward.h include/component/fixed_component.h include/component/component.h src/component/component.cpp src/component/component.cpp)

set_property(TARGET component PROPERTY CXX_STANDARD 20)

target_include_directories(component PUBLIC include)
target_include_directories(component PRIVATE src)

target_link_libraries(component PRIVATE logger maths engine eventbus)
target_link_libraries(component PUBLIC util)