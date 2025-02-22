#include "graphics/plugins/mesh_3d_plugin.h"

#include "material_instance_manager.h"
#include "material_manager.h"
#include "mesh_manager.h"
#include "core/runtime.h"
#include "graphics/components/3d/lighting.h"
#include "graphics/plugins/graphics_plugin.h"
#include "graphics/renderlayer/mesh_layer.h"

using namespace phenyl::graphics;

Mesh3DPlugin::Mesh3DPlugin () = default;
Mesh3DPlugin::~Mesh3DPlugin () = default;

std::string_view Mesh3DPlugin::getName () const noexcept {
    return "Mesh3DPlugin";
}

void Mesh3DPlugin::init (core::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();

    auto& renderer = runtime.resource<Renderer>();
    materialManager = std::make_unique<MaterialManager>(renderer);
    materialManager->selfRegister();

    materialInstanceManager = std::make_unique<MaterialInstanceManager>();
    materialInstanceManager->selfRegister();

    meshManager = std::make_unique<MeshManager>(renderer);
    meshManager->selfRegister();

    auto& layer = renderer.addLayer<MeshRenderLayer>(runtime.world());
    layer.addSystems(runtime);

    runtime.addComponent<MeshRenderer3D>("MeshRenderer3D");
    runtime.addComponent<PointLight3D>("PointLight3D");
    runtime.addComponent<DirectionalLight3D>("DirectionalLight3D");
    runtime.addComponent<SpotLight3D>("SpotLight3D");
}
