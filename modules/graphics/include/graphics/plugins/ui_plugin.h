#pragma once

#include "graphics/ui/ui_manager.h"
#include "runtime/plugin.h"

namespace phenyl::graphics {
    class UIPlugin : public runtime::IPlugin {
    private:
        std::unique_ptr<UIManager> manager;
    public:
        std::string_view getName () const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
        void frameBegin(runtime::PhenylRuntime& runtime) override;
        void render(runtime::PhenylRuntime &runtime) override;
    };
}