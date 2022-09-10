add_library(engine include src src/engine/game_init.cpp src/engine/entity/controller/entity_controller.cpp src/engine/entity/entities.cpp src/engine/entity/entity.cpp src/engine/entity/entity_type.cpp src/engine/map/map.cpp src/engine/map/map_reader.cpp src/engine/tile/tile.cpp src/engine/game_camera.cpp src/engine/game_object.cpp include/engine/engine.h src/engine/engine.cpp include/engine/phenyl_game.h src/engine/phenyl_game.cpp include/engine/input/game_input.h src/engine/input/game_input.cpp include/engine/entity/entity_type_new.h src/engine/entity/entity_type_new.cpp)

#find_package(nlohmann_json 3.2.0 REQUIRED)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)

target_include_directories(engine PUBLIC include)
target_include_directories(engine PRIVATE src)

target_link_libraries(engine PRIVATE logger component util maths eventbus graphics physics common)