add_library(platform OBJECT)
set_property(TARGET platform PROPERTY CXX_STANDARD 20)

if(UNIX)
    target_sources(platform PRIVATE src/unix/dynamic_library.cpp)
    target_include_directories(platform PRIVATE src/unix)
endif()

target_include_directories(platform PUBLIC include)
target_link_libraries(platform PRIVATE logger)
