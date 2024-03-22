add_library(runtime
        include/runtime/runtime.h
        include/runtime/iresource.h
        src/runtime.cpp
        include/runtime/init_plugin.h)

set_property(TARGET runtime PROPERTY CXX_STANDARD 20)

target_include_directories(runtime PRIVATE src)
target_include_directories(runtime PUBLIC include)

target_link_libraries(runtime PUBLIC component util logger common)

