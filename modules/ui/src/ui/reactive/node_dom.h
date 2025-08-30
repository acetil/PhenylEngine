#pragma once
#include "ui/reactive/node.h"

#include <unordered_map>

namespace phenyl::graphics {
class UINodeDOM {
public:
    UINode* pick (glm::vec2 pointer) const;
    void pointerUpdate (glm::vec2 pointer);

    void measure (const WidgetConstraints& constraints);
    void render (Canvas& canvas);

    void insert (std::size_t id, UINode* node);
    void remove (std::size_t id);
    void setRoot (std::unique_ptr<UINode> root);

    UINode* get (std::size_t id) const;

    void clear ();

private:
    std::unique_ptr<UINode> m_root;
    std::unordered_map<std::size_t, detail::UINodeState> m_nodeStates;
    std::unordered_map<std::size_t, UINode*> m_nodes;
};
} // namespace phenyl::graphics
