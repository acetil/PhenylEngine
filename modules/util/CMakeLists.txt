add_library(util src/data.cpp src/profiler.cpp src/string_help.cpp src/test_data.cpp include/util/optional.h include/util/detail/memory.h include/util/fixed_stack.h include/util/set.h src/set_test.cpp include/util/fake_bool.h include/util/callback_member.h include/util/bitfield.h include/util/fl_vector.h include/util/game_id.h include/util/iterable.h
        include/util/random.h
        src/random.cpp)

find_package(nlohmann_json REQUIRED)

set_property(TARGET util PROPERTY CXX_STANDARD 20)

target_include_directories(util PUBLIC include)
target_include_directories(util PRIVATE src)

target_link_libraries(util PRIVATE logger nlohmann_json::nlohmann_json)