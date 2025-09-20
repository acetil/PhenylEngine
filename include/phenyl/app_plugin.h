#pragma once

#include "core/plugin.h"

#include <memory>

namespace phenyl::engine {
class ApplicationBase;

class AppPlugin : public core::IPlugin {
public:
    explicit AppPlugin (std::unique_ptr<ApplicationBase> app);

    [[nodiscard]] std::string_view getName () const noexcept override;
    void init (core::PhenylRuntime& runtime) override;
    // void update (runtime::PhenylRuntime &runtime, double deltaTime) override;
    // void fixedUpdate (runtime::PhenylRuntime &runtime, double deltaTime) override;
    void shutdown (core::PhenylRuntime& runtime) override;

private:
    std::unique_ptr<ApplicationBase> m_app;
};
} // namespace phenyl::engine
