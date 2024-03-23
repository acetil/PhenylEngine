#pragma once

#include "runtime/init_plugin.h"

namespace phenyl::graphics {
    class Sprite2DPlugin : public runtime::IInitPlugin {
    public:
        Sprite2DPlugin () = default;

        std::string_view getName () const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
    };
}