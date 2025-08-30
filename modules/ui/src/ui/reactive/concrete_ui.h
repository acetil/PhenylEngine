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
    std::size_t depth;
    UINode* node;
};

class ConcreteUI : public UI {
public:
    ConcreteUI (core::GameInput& input);
    ~ConcreteUI () override;

    void markDirty (std::size_t id) override;
    UIRoot& root () override;
    std::size_t makeId () override;
    void onComponentDestroy (std::size_t id) override;

    void handleInput ();
    void update ();
    void canvasRender (Canvas& canvas);

    void insertComponent (UIComponentNode* node);
    void insertNode (UIComponentBase* comp, UINode* node);
    void onComponentRender (UIComponentBase* comp);

private:
    struct ParentNode {
        UINode* node = nullptr;
        std::size_t depth = 0;
        std::optional<std::size_t> parentIndex = std::nullopt;
    };

    std::unique_ptr<UIRoot> m_root;

    std::unique_ptr<UIComponentNode> m_rootComp;
    std::unordered_map<std::size_t, UIComponentNode*> m_components;
    std::unordered_set<std::size_t> m_dirtyComps;
    std::unordered_set<UIComponentBase*> m_renderedComps;

    bool m_doReMeasure = true;

    UIComponentNode* m_current = nullptr;

    std::unique_ptr<UINodeDOM> m_nodeDom;

    // TODO: refactor
    core::InputAction m_selectAction;
    core::Axis2DInput m_mousePos;
    bool m_mouseDown = false;
    std::optional<std::size_t> m_focusedNode;

    std::size_t m_nextId = 1;

    void refreshRender ();
    void rerenderComponent (UIComponentNode* node);
};
} // namespace phenyl::graphics
