#pragma once

#include "runtime/plugin.h"

namespace phenyl::physics {
    class Physics2D;

    class Physics2DPlugin : public runtime::IPlugin {
    private:
        std::unique_ptr<Physics2D> physics;
    public:
        Physics2DPlugin ();
        ~Physics2DPlugin() override;

        [[nodiscard]] std::string_view getName() const noexcept override;

        void init(runtime::PhenylRuntime& runtime) override;
        void render(runtime::PhenylRuntime& runtime);
    };
}
