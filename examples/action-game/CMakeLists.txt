add_executable(action-game src/main.cpp src/util/debug_console.cpp
        src/game/entity/bullet.h
        src/game/entity/player.h
        src/game/entity/bullet.cpp
        src/game/entity/player.cpp
        src/game/test_app.h
        src/game/test_app.cpp)

set_property(TARGET action-game PROPERTY CXX_STANDARD 20)
target_compile_options(action-game PRIVATE "-g")

target_copy_resources(action-game
        resources/images/test/bullet2.png
        resources/images/test/manBlue_gun.png
        resources/images/test/test_texture.png
        resources/images/test/test_wall.png
        resources/levels/test_level.json
        resources/prefabs/block_entity.json
        resources/prefabs/bullet_entity.json
        resources/prefabs/test_entity.json
        resources/prefabs/wall_entity.json
        resources/prefabs/bullet_particle_entity.json
        resources/particles/test_system.json
        resources/audio/blastoff.wav
        resources/fonts/OpenSans-Regular.ttf
        resources/fonts/OFL.txt
)
target_copy_phenyl_resources(action-game)

target_include_directories(action-game PRIVATE src)
target_link_libraries(action-game PRIVATE phenyl)