add_library(ui OBJECT include/
        src/ui/debug_ui.cpp
        src/ui/ui_manager.cpp
        src/ui/widgets/button.cpp
        src/ui/widgets/container.cpp
        src/ui/widgets/label.cpp
        src/ui/widgets/layout.cpp
        src/ui/widgets/root.cpp
        src/ui/widgets/widget.cpp
        src/ui/plugins/ui_plugin.cpp
        include/ui/reactive/state.h
        include/ui/reactive/component.h
        include/ui/reactive/ui.h
        include/ui/reactive/node.h
        src/ui/reactive/component.cpp
        src/ui/reactive/concrete_ui.h
        src/ui/reactive/concrete_ui.cpp
        include/ui/reactive/atom.h
        include/ui/reactive/root.h
        src/ui/reactive/root.cpp
        src/ui/reactive/root_comp.h
        src/ui/reactive/root_comp.cpp
)

set_property(TARGET ui PROPERTY CXX_STANDARD 20)

target_include_directories(ui PUBLIC include)
target_include_directories(ui PRIVATE src)

target_link_libraries(ui PUBLIC core graphics util)
target_link_libraries(ui PRIVATE logger)
