#pragma once

#include "ui/reactive/component.h"
#include "ui/reactive/root.h"

namespace phenyl::graphics {
struct UIRootProps {
    UIAtom<detail::UIRootComponents> comps;
};

class UIRootComponent : public UIComponent<UIRootComponent, UIRootProps> {
public:
    UIRootComponent (UI& ui, UIRootProps&& props);

    void render (UI& ui) override;
};
} // namespace phenyl::graphics
