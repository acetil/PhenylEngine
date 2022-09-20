add_library(component src/component/component_extern.cpp src/component/components/2D/rotation.cpp include/component/components/2D/position.h src/component/components/2D/position.cpp src/component/view/game_view.cpp include/component/component_serialiser.h include/component/forward.h)

set_property(TARGET component PROPERTY CXX_STANDARD 20)

target_include_directories(component PUBLIC include)
target_include_directories(component PRIVATE src)

target_link_libraries(component PRIVATE logger maths engine eventbus)
target_link_libraries(component PUBLIC util)