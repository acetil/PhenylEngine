add_executable(breakout src/main.cpp src/breakout.cpp
        src/paddle.cpp)

set_property(TARGET breakout PROPERTY CXX_STANDARD 20)

target_link_libraries(breakout PRIVATE phenyl)
target_copy_resources(breakout
        resources/levels/main.json
        resources/prefabs/paddle.json
        resources/images/paddle.png
        resources/images/test_wall.png
        resources/prefabs/wall.json
)