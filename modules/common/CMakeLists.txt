#add_library(common)
add_library(common include/common include/common/input/input_source.h include/common/input/remappable_input.h src/common/input/remappable_input.cpp include/common/events/debug/reload_theme.h include/common/events/theme_change.h include/common/scope.h include/common/input/proxy_source.h src/common/input/proxy_source.cpp)

set_property(TARGET common PROPERTY CXX_STANDARD 20)

target_include_directories(common PUBLIC include)
#target_include_directories(common INTERFACE include)
target_include_directories(common PRIVATE src)

#target_link_libraries(common PRIVATE logger maths eventbus)
#target_link_libraries(common PUBLIC util)
target_link_libraries(common PUBLIC maths eventbus util)
target_link_libraries(common PRIVATE logger)