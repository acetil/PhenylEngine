add_library(runtime
        include/runtime/runtime.h
        include/runtime/iresource.h
        src/runtime.cpp
        include/runtime/init_plugin.h
        include/runtime/system.h
        include/runtime/forward.h
        include/runtime/resource_manager.h
        include/runtime/stage.h
        include/runtime/delta_time.h
        include/runtime/resources.h
        src/stages.cpp
        include/runtime/stages.h
)

set_property(TARGET runtime PROPERTY CXX_STANDARD 20)

target_include_directories(runtime PRIVATE src)
target_include_directories(runtime PUBLIC include)

target_link_libraries(runtime PUBLIC component util logger common)

