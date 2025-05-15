#pragma once

#include "core/plugin.h"

namespace phenyl::graphics {
class EntityRenderLayer;

class Sprite2DPlugin : public core::IInitPlugin {
public:
    Sprite2DPlugin () = default;

    std::string_view getName () const noexcept override;
    void init (core::PhenylRuntime& runtime) override;

private:
    EntityRenderLayer* m_entityLayer = nullptr;
};
} // namespace phenyl::graphics
