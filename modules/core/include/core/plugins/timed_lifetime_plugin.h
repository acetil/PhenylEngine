#pragma once

#include "core/plugin.h"

namespace phenyl::core {
class TimedLifetimePlugin : public IInitPlugin {
public:
    std::string_view getName () const noexcept override;
    void init (PhenylRuntime& runtime) override;
};
} // namespace phenyl::core
