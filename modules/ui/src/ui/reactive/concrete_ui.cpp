#include "concrete_ui.h"

#include "root_comp.h"
#include "ui/reactive/root.h"

using namespace phenyl::graphics;

std::string_view UI::getName () const noexcept {
    return "phenyl::UI";
}

ConcreteUI::ConcreteUI () {
    auto rootAtom = UIAtom<detail::UIRootComponents>::Make();
    m_root = std::make_unique<UIRoot>(rootAtom);
    auto rootComp = std::make_unique<UIRootComponent>(*this,
        UIRootProps{
          .comps = rootAtom,
        });
    m_rootComp = std::make_unique<UIComponentNode>(std::move(rootComp), nullptr);
    m_doRerender = true;
}

ConcreteUI::~ConcreteUI () = default;

void ConcreteUI::markDirty () {
    m_doRerender = true;
}

UIRoot& ConcreteUI::root () {
    PHENYL_DASSERT(m_root);
    return *m_root;
}

void ConcreteUI::update () {
    if (!m_doRerender) {
        return;
    }

    m_rootNode = nullptr;

    PHENYL_DASSERT(m_rootComp);
    m_rootComp->component->render(*this);
    OnRenderEnd(m_rootComp.get());

    m_doRerender = false;
}

void ConcreteUI::canvasRender (Canvas& canvas) {
    PHENYL_DASSERT(m_rootNode);

    m_rootNode->measure(WidgetConstraints{
      .maxSize = canvas.resolution(),
    });
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
    m_current->children.emplace(key, UIComponentNode{.component = std::move(component), .parent = m_current});
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
}

void ConcreteUI::pop () {
    PHENYL_DASSERT(m_current);

    OnRenderEnd(m_current);
    m_current = m_current->parent;
    PHENYL_DASSERT(m_current);
}

UINode& ConcreteUI::addNode (std::unique_ptr<UINode> node) {
    auto* ptr = node.get();
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
