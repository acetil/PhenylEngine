add_library(breakout SHARED src/breakout.cpp
        src/paddle.cpp
        src/ball.cpp
        src/tile.cpp
        src/wall.h
        src/wall.cpp
        src/controller.h
        src/controller.cpp)
phenyl_executable(breakout)
set_property(TARGET breakout PROPERTY CXX_STANDARD 20)

target_link_libraries(breakout PRIVATE phenyl)
target_copy_resources(breakout
        resources/levels/main.json
        resources/prefabs/paddle.json
        resources/images/paddle.png
        resources/images/test_wall.png
        resources/prefabs/wall.json
        resources/prefabs/ball.json
        resources/images/ball.png
        resources/prefabs/tile.json
        resources/images/tile_1.png
        resources/prefabs/paddle_collision_emitter.json
        resources/particles/paddle_collision.json
        resources/prefabs/wall_collision_emitter.json
        resources/particles/wall_collision.json
        resources/prefabs/tile_collision_emitter.json
        resources/particles/tile_break.json
        resources/audio/hit2.wav
        resources/audio/slap.wav
        resources/audio/bounce.wav
        resources/prefabs/floor.json
        resources/audio/fail.wav
)
target_copy_phenyl_resources(breakout)
