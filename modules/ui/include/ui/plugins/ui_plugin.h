#pragma once

#include "core/plugin.h"
#include "ui/ui_manager.h"

namespace phenyl::graphics {
class ConcreteUI;

class UIPlugin : public core::IPlugin {
public:
    UIPlugin ();
    ~UIPlugin () override;

    std::string_view getName () const noexcept override;
    void init (core::PhenylRuntime& runtime) override;

private:
    std::unique_ptr<UIManager> m_manager;
    std::unique_ptr<ConcreteUI> m_concreteUi;
};
} // namespace phenyl::graphics
