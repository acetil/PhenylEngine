#pragma once

#include "core/plugin.h"
#include "graphics/ui/ui_manager.h"

namespace phenyl::graphics {
class UIPlugin : public core::IPlugin {
public:
    std::string_view getName () const noexcept override;
    void init (core::PhenylRuntime& runtime) override;

private:
    std::unique_ptr<UIManager> m_manager;
};
} // namespace phenyl::graphics
