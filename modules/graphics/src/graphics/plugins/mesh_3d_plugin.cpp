#include "graphics/plugins/mesh_3d_plugin.h"

#include "mesh_manager.h"
#include "core/runtime.h"
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
    meshManager = std::make_unique<MeshManager>(renderer);
    meshManager->selfRegister();

    auto& layer = renderer.addLayer<MeshRenderLayer>(runtime.world());
    layer.addSystems(runtime);

    runtime.addComponent<MeshRenderer3D>("MeshRenderer3D");
}

