#pragma once

#include "core/iresource.h"

namespace phenyl::graphics {
    class EntityRenderLayer;

    class Sprite2DPlugin : public core::IInitPlugin {
    private:
        EntityRenderLayer* entityLayer = nullptr;
    public:
        Sprite2DPlugin () = default;

        std::string_view getName () const noexcept override;
        void init (core::PhenylRuntime& runtime) override;
    };
}