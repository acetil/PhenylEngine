add_library(graphics_api INTERFACE include)

set_property(TARGET graphics_api PROPERTY CXX_STANDARD 20)

target_include_directories(graphics_api INTERFACE include)

target_link_libraries(graphics_api INTERFACE core maths logger)
