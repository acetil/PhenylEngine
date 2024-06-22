#pragma once

#include "runtime/plugin.h"

namespace phenyl::graphics {
    class EntityRenderLayer;

    class Sprite2DPlugin : public runtime::IInitPlugin {
    private:
        EntityRenderLayer* entityLayer = nullptr;
    public:
        Sprite2DPlugin () = default;

        std::string_view getName () const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
    };
}