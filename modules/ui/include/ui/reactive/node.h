#pragma once

#include "graphics/canvas/canvas.h"
#include "ui/widgets/widget.h"

namespace phenyl::graphics {
class UINode {
public:
    explicit UINode (const Modifier& modifier) : m_modifier{modifier} {}

    virtual ~UINode () = default;

    const Modifier& modifier () const noexcept {
        return m_modifier;
    }

    virtual void measure (const WidgetConstraints& constraints) = 0;
    virtual void render (Canvas& canvas) = 0;

    virtual void addChild (std::unique_ptr<UINode> node) = 0;

private:
    Modifier m_modifier;
};
} // namespace phenyl::graphics
