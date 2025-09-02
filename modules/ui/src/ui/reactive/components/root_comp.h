#pragma once

#include "ui/reactive/component.h"
#include "ui/reactive/components/root.h"

namespace phenyl::graphics {
struct UIRootProps {
    UIAtom<detail::UIRootComponents> comps;
};

class UIRootComponent : public UIComponent<UIRootComponent, UIRootProps> {
public:
    UIRootComponent (UI& ui, UIRootProps&& props);

    UIRenderResult render (UIContext& ctx) const override;
};
} // namespace phenyl::graphics
