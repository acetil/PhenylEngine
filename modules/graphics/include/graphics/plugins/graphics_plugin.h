#pragma once

#include <memory>

#include "../../../backends/api/include/graphics/viewport.h"
#include "core/plugin.h"

namespace phenyl::graphics {
    class DebugLayer;
    class TextureManager;

    class GraphicsPlugin : public core::IPlugin {
    private:
        std::unique_ptr<TextureManager> textureManager;
        //DebugLayer* debugLayer = nullptr;

    public:
        GraphicsPlugin ();
        ~GraphicsPlugin () override;
        [[nodiscard]] std::string_view getName() const noexcept override;
        void init (core::PhenylRuntime& runtime) override;
        void postInit (core::PhenylRuntime& runtime);
    };
}