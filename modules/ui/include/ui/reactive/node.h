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

    UINode* parent () const noexcept;
    std::size_t parentIndex () const noexcept;
    void setParent (UINode* parent, std::size_t parentIndex);
    void setState (detail::UINodeState* state);

    virtual UINode* pick (glm::vec2 pointer) noexcept;
    virtual bool doPointerUpdate (glm::vec2 pointer);
    virtual void onPointerLeave ();

    virtual bool raise (const UIEvent& event);

    virtual void measure (const WidgetConstraints& constraints);
    virtual void render (Canvas& canvas) = 0;

    virtual void addChild (std::unique_ptr<UINode> node) = 0;
    virtual void replaceChild (std::size_t index, std::unique_ptr<UINode> node) = 0;

protected:
    void setDimensions (glm::vec2 dimensions);
    bool handle (const UIEvent& event);

private:
    Modifier m_modifier;
    glm::vec2 m_dimensions{0, 0};
    UINode* m_parent = nullptr;
    std::size_t m_parentIndex = -1;

    detail::UINodeState* m_state = nullptr;

    bool bubbleUp (const UIEvent& event);
};

class EmptyUINode : public UINode {
public:
    EmptyUINode ();

    UINode* pick (glm::vec2 pointer) noexcept override;
    bool doPointerUpdate (glm::vec2 pointer) override;
    void measure (const WidgetConstraints& constraints) override;
    void render (Canvas& canvas) override;

    void addChild (std::unique_ptr<UINode> node) override;
    void replaceChild (std::size_t index, std::unique_ptr<UINode> node) override;
};
} // namespace phenyl::graphics
