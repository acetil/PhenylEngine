add_library(model-viewer SHARED src/model_viewer.cpp)

phenyl_executable(model-viewer)
set_property(TARGET model-viewer PROPERTY CXX_STANDARD 20)

target_link_libraries(model-viewer PRIVATE phenyl)

target_copy_resources(model-viewer
    resources/meshes/cube.obj
    resources/meshes/suzanne.obj

    resources/material_instances/mat1.json
    resources/material_instances/mat2.json
)
target_copy_phenyl_resources(model-viewer)