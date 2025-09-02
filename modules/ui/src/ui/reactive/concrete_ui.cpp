#include "concrete_ui.h"

#include "components/root_comp.h"
#include "core/input/game_input.h"
#include "ui/reactive/components/root.h"
#include "ui/reactive/render.h"

using namespace phenyl::graphics;

std::string_view UI::getName () const noexcept {
    return "phenyl::UI";
}

ConcreteUI::ConcreteUI (core::GameInput& input) :
    m_selectAction{input.addAction("ui_select2")},
    m_mousePos{input.addAxis2D("ui_mouse2")} {
    input.addActionBinding("ui_select2", "mouse.button_left");
    input.addAxis2DBinding("ui_mouse2", "mouse.mouse_pos");

    auto rootAtom = UIAtom<detail::UIRootComponents>::Make();
    m_root = std::make_unique<UIRoot>(rootAtom);
    auto rootComp = std::make_unique<UIRootComponent>(*this,
        UIRootProps{
          .comps = rootAtom,
        });
    m_rootComp = std::make_unique<UIComponentNode>(std::move(rootComp), nullptr);
    insertComponent(m_rootComp.get());
    m_current = m_rootComp.get();

    m_nodeDom = std::make_unique<UINodeDOM>();
    auto result = UIContext{*this, m_rootComp.get()}.renderComponent();
    m_nodeDom->setRoot(std::move(result).node());
}

ConcreteUI::~ConcreteUI () {
    m_nodeDom = nullptr;
    m_rootComp = nullptr;
    m_root = nullptr;
}

void ConcreteUI::markDirty (std::size_t id) {
    m_dirtyComps.emplace(id);
}

UIRoot& ConcreteUI::root () {
    PHENYL_DASSERT(m_root);
    return *m_root;
}

std::size_t ConcreteUI::makeId () {
    return m_nextId++;
}

void ConcreteUI::onComponentDestroy (std::size_t id) {
    if (m_nodeDom) {
        m_nodeDom->remove(id);
    }
    m_components.erase(id);
}

void ConcreteUI::handleInput () {
    PHENYL_DASSERT(m_nodeDom);
    auto pointer = m_mousePos.value();
    m_nodeDom->pointerUpdate(pointer);

    bool newMouse = m_selectAction.value();
    if (newMouse != m_mouseDown) {
        // TODO: focus
        if (newMouse) {
            auto* node = m_nodeDom->pick(pointer);
            if (node) {
                m_focusedNode = node->id();
                node->raise(UIEvent{MousePressEvent{}});
            }
        } else {
            if (m_focusedNode) {
                auto* node = m_nodeDom->get(*m_focusedNode);
                if (node) {
                    node->raise(UIEvent{MouseReleaseEvent{}});
                }
                m_focusedNode = std::nullopt;
            }
        }
    }
    m_mouseDown = newMouse;
}

void ConcreteUI::update () {
    if (m_dirtyComps.empty()) {
        return;
    }
    refreshRender();
    m_doReMeasure = true;
}

void ConcreteUI::canvasRender (Canvas& canvas) {
    PHENYL_DASSERT(m_nodeDom);

    if (m_doReMeasure) {
        m_nodeDom->measure(WidgetConstraints{
          .maxSize = canvas.resolution(),
        });
        m_doReMeasure = false;
    }
    m_nodeDom->render(canvas);
}

void ConcreteUI::insertComponent (UIComponentNode* node) {
    m_components.emplace(node->component->id(), node);
}

void ConcreteUI::insertNode (UIComponentBase* comp, UINode* node) {
    PHENYL_DASSERT(comp);
    PHENYL_DASSERT(node);
    m_nodeDom->insert(comp->id(), node);
}

void ConcreteUI::onComponentRender (UIComponentBase* comp) {
    m_renderedComps.emplace(comp);
}

void ConcreteUI::refreshRender () {
    // m_nodeDom->clear();
    m_renderedComps.clear();

    // auto result = UIContext{*this, m_rootComp.get()}.renderComponent();
    // auto node = std::move(result).node();
    // // m_nodeDom->insert(m_rootComp->component->id(), node.get());
    //
    // m_nodeDom->setRoot(std::move(node));
    // m_dirtyComps.clear();
    for (auto i : m_dirtyComps) {
        auto it = m_components.find(i);
        if (it != m_components.end()) {
            rerenderComponent(it->second);
        }
    }
    m_dirtyComps.clear();
}

void ConcreteUI::rerenderComponent (UIComponentNode* compNode) {
    if (m_renderedComps.contains(compNode->component.get())) {
        return;
    }
    if (compNode->parent && compNode->parent->node == compNode->node) {
        rerenderComponent(compNode->parent);
        return;
    }

    auto* parent = compNode->node->parent();
    auto parentIndex = compNode->node->parentIndex();
    auto result = UIContext{*this, compNode}.renderComponent();
    if (!parent) {
        m_nodeDom->setRoot(std::move(result).node());
        return;
    }

    parent->replaceChild(parentIndex, std::move(result).node());
}
