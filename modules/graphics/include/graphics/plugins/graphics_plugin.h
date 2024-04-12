#pragma once

#include "graphics/viewport.h"
#include "runtime/plugin.h"

namespace phenyl::graphics {
    class DebugLayer;
    class TextureManager;

    class GraphicsPlugin : public runtime::IPlugin {
    private:
        std::unique_ptr<TextureManager> textureManager;
        DebugLayer* debugLayer = nullptr;

    public:
        GraphicsPlugin ();
        ~GraphicsPlugin () override;
        [[nodiscard]] std::string_view getName() const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
        void render (runtime::PhenylRuntime& runtime) override;
    };
}