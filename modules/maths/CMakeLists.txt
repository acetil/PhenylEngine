add_library(maths include/graphics/maths_headers.h src/maths_headers.cpp)

set_property(TARGET maths PROPERTY CXX_STANDARD 20)

target_include_directories(maths PUBLIC include)
target_include_directories(maths PRIVATE src)

target_link_libraries(maths PRIVATE logger util)
