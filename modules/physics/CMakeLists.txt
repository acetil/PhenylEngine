add_library(physics src/physics/physics.cpp include/physics/components/2D/rigid_body.h src/physics/components/2D/rigid_body.cpp src/physics/2d/physics_2d.h src/physics/2d/physics_2d.cpp
        src/physics/2d/collisions_2d.h
        src/physics/2d/collisions_2d.cpp
        include/physics/components/2D/collider.h
        include/physics/components/2D/collider.h
        src/physics/components/2D/collider.cpp
        include/physics/components/2D/colliders/box_collider.h
        src/physics/components/2D/colliders/box_collider.cpp
        include/physics/signals/collision.h
)

set_property(TARGET physics PROPERTY CXX_STANDARD 20)

target_include_directories(physics PUBLIC include)
target_include_directories(physics PRIVATE src)

target_link_libraries(physics PRIVATE logger util maths core)
target_link_libraries(physics PUBLIC core)