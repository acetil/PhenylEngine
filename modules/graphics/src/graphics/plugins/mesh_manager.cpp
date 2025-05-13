#include "mesh_manager.h"

#include "core/assets/assets.h"
#include "graphics/filetypes/obj_file.h"

using namespace phenyl::graphics;

MeshManager::MeshManager (Renderer& renderer) : m_renderer{renderer} {}

void MeshManager::selfRegister () {
    core::Assets::AddManager(this);
}

Mesh* MeshManager::load (std::ifstream& data, std::size_t id) {
    // Only obj files supported for now

    ObjFile objFile{data};
    auto mesh = objFile.makeMesh(m_renderer);
    auto* ptr = mesh.get();

    if (auto it = m_layoutIds.find(mesh->layout()); it != m_layoutIds.end()) {
        mesh->setLayoutId(it->second);
    } else {
        mesh->setLayoutId(m_nextLayoutId);
        m_layoutIds.emplace(mesh->layout(), m_nextLayoutId++);
    }

    m_meshes.emplace(id, std::move(mesh));
    return ptr;
}

Mesh* MeshManager::load (Mesh&& obj, std::size_t id) {
    PHENYL_ABORT("Virtual loading of mesh assets is not supported!");
}

const char* MeshManager::getFileType () const {
    return "";
}

bool MeshManager::isBinary () const {
    return false;
}

void MeshManager::queueUnload (std::size_t id) {
    // TODO
}
