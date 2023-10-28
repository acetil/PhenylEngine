cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")

add_executable(action-game src/main.cppm)

target_sources(action-game PUBLIC FILE_SET CXX_MODULES FILES
        src/game/entity/player.cppm
        src/game/entity/bullet.cppm
        src/game/test_app.cppm
        src/util/debug_console.cppm
)

#set_property(TARGET action-game PROPERTY CXX_STANDARD 20)
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
        resources/themes/alt_theme.json
        resources/audio/blastoff.wav
)

target_include_directories(action-game PRIVATE src)
target_link_libraries(action-game PRIVATE phenyl)