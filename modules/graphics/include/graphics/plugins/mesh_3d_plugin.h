#pragma once

#include <memory>

#include "core/plugin.h"

namespace phenyl::graphics {
    class MeshManager;
    class MaterialManager;
    class MaterialInstanceManager;

    class Mesh3DPlugin : public core::IPlugin {
    private:
        std::unique_ptr<MaterialManager> materialManager;
        std::unique_ptr<MaterialInstanceManager> materialInstanceManager;
        std::unique_ptr<MeshManager> meshManager;
    public:
        Mesh3DPlugin ();
        ~Mesh3DPlugin();
        std::string_view getName() const noexcept override;

        void init(core::PhenylRuntime& runtime) override;
    };
}
