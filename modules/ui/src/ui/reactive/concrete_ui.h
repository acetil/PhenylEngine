#pragma once
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
    ConcreteUI ();
    ~ConcreteUI () override;

    void markDirty () override;
    UIRoot& root () override;

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
    static void OnRenderEnd (UIComponentNode* curr);
    std::unique_ptr<UIRoot> m_root;

    std::unique_ptr<UIComponentNode> m_rootComp;
    bool m_doRerender = false;

    UIComponentNode* m_current = nullptr;

    std::unique_ptr<UINode> m_rootNode = nullptr;
    std::vector<UINode*> m_currNodes;
};
} // namespace phenyl::graphics
