add_library(ui OBJECT include/
        src/ui/debug_ui.cpp
        src/ui/ui_manager.cpp
        src/ui/widgets/button.cpp
        src/ui/widgets/container.cpp
        src/ui/widgets/label.cpp
        src/ui/widgets/layout.cpp
        src/ui/widgets/root.cpp
        src/ui/widgets/widget.cpp
        src/ui/plugins/ui_plugin.cpp)

set_property(TARGET ui PROPERTY CXX_STANDARD 20)

target_include_directories(ui PUBLIC include)
target_include_directories(ui PRIVATE src)

target_link_libraries(ui PUBLIC core graphics)
target_link_libraries(ui PRIVATE logger)
