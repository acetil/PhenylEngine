#pragma once

#include "core/plugin.h"

#include <memory>

namespace phenyl::graphics {
class MeshManager;
class MaterialManager;
class MaterialInstanceManager;

class Mesh3DPlugin : public core::IPlugin {
public:
    Mesh3DPlugin ();
    ~Mesh3DPlugin ();
    std::string_view getName () const noexcept override;

    void init (core::PhenylRuntime& runtime) override;

private:
    std::unique_ptr<MaterialManager> m_materialManager;
    std::unique_ptr<MaterialInstanceManager> m_materialInstanceManager;
    std::unique_ptr<MeshManager> m_meshManager;
};
} // namespace phenyl::graphics
