add_library(component src/component/main_component.cpp include/component/main_component.h src/component/component_extern.cpp src/component/rotation_component.cpp include/component/component_new.h include/component/view/entity_view.h include/component/view/constrained_view.h)

set_property(TARGET component PROPERTY CXX_STANDARD 20)

target_include_directories(component PUBLIC include)
target_include_directories(component PRIVATE src)

target_link_libraries(component PRIVATE logger maths)
target_link_libraries(component PUBLIC util)