#pragma once

#include "graphics/canvas/canvas.h"
#include "ui/widgets/widget.h"

namespace phenyl::graphics {
namespace detail {
    struct UINodeState {
        std::size_t id;
        std::optional<glm::vec2> oldPointerPos;
    };
} // namespace detail

class UINode {
public:
    explicit UINode (Modifier modifier);
    virtual ~UINode () = default;

    const Modifier& modifier () const noexcept {
        return m_modifier;
    }

    glm::vec2 dimensions () const noexcept {
        return m_dimensions;
    }

    std::size_t id () const noexcept {
        PHENYL_DASSERT(m_state);
        return m_state->id;
    }

    void setParent (UINode* parent);
    void setState (detail::UINodeState* state);

    virtual UINode* pick (glm::vec2 pointer) noexcept;
    virtual bool doPointerUpdate (glm::vec2 pointer);
    virtual void onPointerLeave ();

    virtual bool raise (const UIEvent& event);

    virtual void measure (const WidgetConstraints& constraints);
    virtual void render (Canvas& canvas) = 0;

    virtual void addChild (std::unique_ptr<UINode> node) = 0;

protected:
    void setDimensions (glm::vec2 dimensions);
    bool handle (const UIEvent& event);

private:
    Modifier m_modifier;
    glm::vec2 m_dimensions{0, 0};
    UINode* m_parent = nullptr;

    detail::UINodeState* m_state = nullptr;

    bool bubbleUp (const UIEvent& event);
};
} // namespace phenyl::graphics
