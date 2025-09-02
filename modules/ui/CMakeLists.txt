add_library(ui OBJECT
        src/ui/debug_ui.cpp
        src/ui/plugins/ui_plugin.cpp
        include/ui/reactive/state.h
        include/ui/reactive/component.h
        include/ui/reactive/ui.h
        include/ui/reactive/node.h
        src/ui/reactive/component.cpp
        src/ui/reactive/concrete_ui.h
        src/ui/reactive/concrete_ui.cpp
        include/ui/reactive/atom.h
        include/ui/reactive/components/root.h
        src/ui/reactive/components/root.cpp
        src/ui/reactive/components/root_comp.h
        src/ui/reactive/components/root_comp.cpp
        include/ui/reactive/components/label.h
        src/ui/reactive/components/label.cpp
        include/ui/reactive/components/layout.h
        src/ui/reactive/components/layout.cpp
        include/ui/reactive/components/container.h
        src/ui/reactive/components/container.cpp
        src/ui/reactive/node.cpp
        include/ui/reactive/components/button.h
        src/ui/reactive/components/button.cpp
        src/ui/reactive/node_dom.h
        src/ui/reactive/node_dom.cpp
        include/ui/reactive/render.h
        src/ui/reactive/render.cpp
        include/ui/ui_types.h
)

set_property(TARGET ui PROPERTY CXX_STANDARD 20)

target_include_directories(ui PUBLIC include)
target_include_directories(ui PRIVATE src)

target_link_libraries(ui PUBLIC core graphics util)
target_link_libraries(ui PRIVATE logger)
