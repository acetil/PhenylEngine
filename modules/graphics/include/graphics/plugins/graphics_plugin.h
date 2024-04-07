#pragma once

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
        void render (phenyl::runtime::PhenylRuntime &runtime) override;
        void shutdown (runtime::PhenylRuntime& runtime) override;
    };
}