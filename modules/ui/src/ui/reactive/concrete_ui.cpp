#include "concrete_ui.h"

#include "components/root_comp.h"
#include "core/input/game_input.h"
#include "ui/reactive/components/root.h"

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
    m_current = m_rootComp.get();
    refreshRender();
}

ConcreteUI::~ConcreteUI () {
    m_rootNode = nullptr;
    m_rootComp = nullptr;
    m_root = nullptr;
}

void ConcreteUI::markDirty () {
    m_doRerender = true;
}

UIRoot& ConcreteUI::root () {
    PHENYL_DASSERT(m_root);
    return *m_root;
}

std::size_t ConcreteUI::makeId () {
    return m_nextId++;
}

void ConcreteUI::onComponentDestroy (std::size_t id) {
    m_nodeStates.erase(id);
}

void ConcreteUI::handleInput () {
    PHENYL_DASSERT(m_rootNode);
    auto pointer = m_mousePos.value();
    m_rootNode->doPointerUpdate(pointer);

    bool newMouse = m_selectAction.value();
    if (newMouse != m_mouseDown) {
        // TODO: focus
        if (newMouse) {
            auto* node = m_rootNode->pick(pointer);
            if (node) {
                m_focusedNode = node->id();
                node->raise(UIEvent{MousePressEvent{}});
            }
        } else {
            if (m_focusedNode) {
                auto it = m_nodes.find(*m_focusedNode);
                if (it != m_nodes.end()) {
                    it->second->raise(UIEvent{MouseReleaseEvent{}});
                }
                m_focusedNode = std::nullopt;
            }
        }
    }
    m_mouseDown = newMouse;
}

void ConcreteUI::update () {
    if (!m_doRerender) {
        return;
    }
    //
    // m_rootNode = nullptr;
    //
    // PHENYL_DASSERT(m_rootComp);
    // m_rootComp->component->render(*this);
    // OnRenderEnd(m_rootComp.get());
    // PHENYL_DASSERT(m_current == m_rootComp.get());
    //
    // m_doRerender = false;
    refreshRender();
    m_doReMeasure = true;
}

void ConcreteUI::canvasRender (Canvas& canvas) {
    PHENYL_DASSERT(m_rootNode);

    if (m_doReMeasure) {
        m_rootNode->measure(WidgetConstraints{
          .maxSize = canvas.resolution(),
        });
        m_doReMeasure = false;
    }
    m_rootNode->render(canvas);
}

UIComponentBase* ConcreteUI::current (std::size_t key) {
    PHENYL_DASSERT(m_current);
    auto it = m_current->children.find(key);
    return it != m_current->children.end() ? it->second.component.get() : nullptr;
}

UIComponentBase* ConcreteUI::setCurrent (std::size_t key, std::unique_ptr<UIComponentBase> component) {
    PHENYL_DASSERT(component);
    PHENYL_DASSERT(m_current);

    auto* ptr = component.get();
    m_current->children.emplace(key,
        UIComponentNode{
          .component = std::move(component),
          .parent = m_current,
        });
    return ptr;
}

std::size_t ConcreteUI::currentSize () const {
    PHENYL_DASSERT(m_current);
    return m_current->seenChildren.size();
}

void ConcreteUI::pushComp (std::size_t key) {
    PHENYL_DASSERT(m_current);

    auto it = m_current->children.find(key);
    PHENYL_DASSERT(it != m_current->children.end());
    m_current->seenChildren.emplace(key);
    m_current = &it->second;

    m_currNodes.emplace_back(m_currNodes.back());
}

void ConcreteUI::pop () {
    PHENYL_DASSERT(m_current);

    OnRenderEnd(m_current);
    m_current = m_current->parent;
    PHENYL_DASSERT(m_current);

    PHENYL_DASSERT(!m_currNodes.empty());
    m_currNodes.pop_back();
}

UINode& ConcreteUI::addNode (std::unique_ptr<UINode> node) {
    PHENYL_DASSERT(node);
    PHENYL_DASSERT(m_current);
    auto* ptr = node.get();
    auto id = m_current->component->id();
    ptr->setState(&m_nodeStates[id]);
    m_nodeStates[id].id = id;
    m_nodes[id] = ptr;

    if (!m_rootNode) {
        m_rootNode = std::move(node);
        m_currNodes.emplace_back(m_rootNode.get());
        return *ptr;
    }

    m_currNodes.back()->addChild(std::move(node));
    m_currNodes.back() = ptr;
    return *ptr;
}

void ConcreteUI::OnRenderEnd (UIComponentNode* curr) {
    PHENYL_DASSERT(curr);

    std::erase_if(curr->children, [&] (const auto& item) { return !curr->seenChildren.contains(item.first); });
    curr->seenChildren.clear();
}

void ConcreteUI::refreshRender () {
    m_rootNode = nullptr;
    m_nodes.clear();

    PHENYL_DASSERT(m_rootComp);
    m_rootComp->component->render(*this);
    OnRenderEnd(m_rootComp.get());
    PHENYL_DASSERT(m_current == m_rootComp.get());

    m_doRerender = false;
}
