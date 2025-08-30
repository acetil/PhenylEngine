#pragma once

#include "core/input/axis_action.h"
#include "core/input/input_action.h"
#include "node_dom.h"
#include "ui/reactive/ui.h"

#include <unordered_set>

namespace phenyl::graphics {
struct UIComponentNode {
    std::unique_ptr<UIComponentBase> component;
    UIComponentNode* parent;
    std::unordered_map<std::size_t, UIComponentNode> children;
    std::unordered_set<std::size_t> seenChildren;
};

class ConcreteUI : public UI {
public:
    ConcreteUI (core::GameInput& input);
    ~ConcreteUI () override;

    void markDirty () override;
    UIRoot& root () override;
    std::size_t makeId () override;
    void onComponentDestroy (std::size_t id) override;

    void handleInput ();
    void update ();
    void canvasRender (Canvas& canvas);

protected:
    UIComponentBase* current (std::size_t key) override;
    UIComponentBase* setCurrent (std::size_t key, std::unique_ptr<UIComponentBase> component) override;
    [[nodiscard]] std::size_t currentSize () const override;

    void pushComp (std::size_t key) override;
    void pop () override;

    UINode& addNode (std::unique_ptr<UINode> node) override;

private:
    std::unique_ptr<UIRoot> m_root;

    std::unique_ptr<UIComponentNode> m_rootComp;
    bool m_doRerender = false;
    bool m_doReMeasure = true;

    UIComponentNode* m_current = nullptr;

    std::unique_ptr<UINodeDOM> m_nodeDom;
    std::vector<UINode*> m_currNodes;

    // TODO: refactor
    core::InputAction m_selectAction;
    core::Axis2DInput m_mousePos;
    bool m_mouseDown = false;
    std::optional<std::size_t> m_focusedNode;

    std::size_t m_nextId = 1;

    void onRenderEnd (UIComponentNode* curr);
    void refreshRender ();
};
} // namespace phenyl::graphics
