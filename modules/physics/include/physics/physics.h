#pragma once

#include "core/plugin.h"

#include <memory>

namespace phenyl::physics {
class Physics2D;

class Physics2DPlugin : public core::IPlugin {
public:
    Physics2DPlugin ();
    ~Physics2DPlugin () override;

    [[nodiscard]] std::string_view getName () const noexcept override;

    void init (core::PhenylRuntime& runtime) override;
    void render (core::PhenylRuntime& runtime);

private:
    std::unique_ptr<Physics2D> m_physics;
};
} // namespace phenyl::physics
