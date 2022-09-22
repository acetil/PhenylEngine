add_library(physics src/collisions.cpp src/collisions.h src/components/simple_friction.cpp include/physics/phenyl_physics.h src/phenyl_physics.cpp src/physics_obj.cpp include/physics/components/2D/kinematic_motion.h src/components/2D/kinematic_motion.cpp)

set_property(TARGET physics PROPERTY CXX_STANDARD 20)

target_include_directories(physics PUBLIC include)
target_include_directories(physics PRIVATE src)

target_link_libraries(physics PRIVATE logger util eventbus maths graphics engine common component)
target_link_libraries(physics PUBLIC component)

