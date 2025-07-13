#include "mesh_manager.h"

#include "core/assets/assets.h"
#include "graphics/filetypes/obj_file.h"

using namespace phenyl::graphics;

MeshManager::MeshManager (Renderer& renderer) : m_renderer{renderer} {}

void MeshManager::selfRegister () {
    core::Assets::AddManager(this);
}

std::shared_ptr<Mesh> MeshManager::load (core::AssetLoadContext& ctx) {
    return ctx.read([&] (std::istream& data) {
        // Only obj files supported for now
        ObjFile objFile{data};
        auto mesh = objFile.makeMesh(m_renderer);

        if (auto it = m_layoutIds.find(mesh->layout()); it != m_layoutIds.end()) {
            mesh->setLayoutId(it->second);
        } else {
            mesh->setLayoutId(m_nextLayoutId);
            m_layoutIds.emplace(mesh->layout(), m_nextLayoutId++);
        }
        return mesh;
    });
}
