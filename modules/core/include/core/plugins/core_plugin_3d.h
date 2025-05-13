#pragma once
#include <core/plugin.h>

namespace phenyl::core {
class Core3DPlugin : public IInitPlugin {
public:
    Core3DPlugin () = default;
    std::string_view getName () const noexcept override;
    void init (PhenylRuntime& runtime) override;
};
} // namespace phenyl::core
