add_library(physics src/collisions.cpp src/collisions.h src/physics.cpp src/simple_friction.cpp)

set_property(TARGET physics PROPERTY CXX_STANDARD 20)

target_include_directories(physics PUBLIC include)
target_include_directories(physics PRIVATE src)

target_link_libraries(physics PRIVATE logger util eventbus maths graphics engine common component)
target_link_libraries(physics PUBLIC component)

