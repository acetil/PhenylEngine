#pragma once

#include <memory>

#include "core/plugin.h"

namespace phenyl::graphics {
    class MeshManager;

    class Mesh3DPlugin : public core::IPlugin {
    private:
        std::unique_ptr<MeshManager> meshManager;
    public:
        Mesh3DPlugin ();
        ~Mesh3DPlugin();
        std::string_view getName() const noexcept override;

        void init(core::PhenylRuntime& runtime) override;
    };
}
