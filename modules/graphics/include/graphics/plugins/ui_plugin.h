#pragma once

#include "graphics/ui/ui_manager.h"
#include "core/plugin.h"

namespace phenyl::graphics {
    class UIPlugin : public core::IPlugin {
    private:
        std::unique_ptr<UIManager> m_manager;
    public:
        std::string_view getName () const noexcept override;
        void init (core::PhenylRuntime& runtime) override;
    };
}