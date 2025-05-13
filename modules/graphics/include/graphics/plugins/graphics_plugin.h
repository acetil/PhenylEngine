#pragma once

#include "../../../backends/api/include/graphics/viewport.h"
#include "core/plugin.h"

#include <memory>

namespace phenyl::graphics {
class DebugLayer;
class TextureManager;

class GraphicsPlugin : public core::IPlugin {
public:
    GraphicsPlugin ();
    ~GraphicsPlugin () override;
    [[nodiscard]] std::string_view getName () const noexcept override;
    void init (core::PhenylRuntime& runtime) override;
    void postInit (core::PhenylRuntime& runtime);

private:
    std::unique_ptr<TextureManager> m_textureManager;
};
} // namespace phenyl::graphics
