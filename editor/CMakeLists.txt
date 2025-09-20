add_executable(editor-introspection
        introspection/src/main.cpp
        introspection/src/mock_clock.h
        introspection/src/mock_renderer.h)
set_property(TARGET editor-introspection PROPERTY CXX_STANDARD 20)

find_package(nlohmann_json REQUIRED)

target_link_libraries(editor-introspection PRIVATE phenyl)
target_link_libraries(editor-introspection PRIVATE nlohmann_json::nlohmann_json)
