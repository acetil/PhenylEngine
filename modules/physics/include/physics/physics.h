#pragma once

#include "component/component.h"
#include "component/forward.h"
#include "runtime/plugin.h"

namespace phenyl::physics {
    class Physics2D;

    class PhysicsPlugin2D : public runtime::IPlugin {
    private:
        std::unique_ptr<Physics2D> physics;
    public:
        PhysicsPlugin2D ();
        ~PhysicsPlugin2D() override;

        [[nodiscard]] std::string_view getName() const noexcept override;

        void init(runtime::PhenylRuntime& runtime) override;
        void physicsUpdate(runtime::PhenylRuntime &runtime, double deltaTime) override;
        void render(runtime::PhenylRuntime& runtime) override;
    };
}
