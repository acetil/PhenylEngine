add_library(physics include/physics/phenyl_physics.h src/physics/phenyl_physics.cpp src/physics/physics_obj.cpp include/physics/components/2D/rigid_body.h src/physics/components/2D/rigid_body.cpp include/physics/physics_core.h include/physics/shape/physics_shape.h src/physics/shape/physics_shape.cpp src/physics/2d/physics_obj_2d.h src/physics/2d/physics_obj_2d.cpp src/physics/2d/collider_2d.h src/physics/2d/shapes/shape_2d.h src/physics/2d/shapes/shape_2d.cpp src/physics/2d/shapes/box_shape_2d.h src/physics/2d/shapes/box_shape_2d.cpp src/physics/2d/shape_registry_2d.h src/physics/shape/shape_registry.cpp include/physics/shape/2d/box_shape_2d_interface.h
        src/physics/2d/shapes/collisions_2d.h
        src/physics/2d/shapes/collisions_2d.cpp)

set_property(TARGET physics PROPERTY CXX_STANDARD 20)

target_include_directories(physics PUBLIC include)
target_include_directories(physics PRIVATE src)

target_link_libraries(physics PRIVATE logger util eventbus maths graphics engine common component)
target_link_libraries(physics PUBLIC component)

