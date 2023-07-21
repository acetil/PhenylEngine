#pragma once

#include <concepts>
#include <memory>
#include <utility>

#include "graphics/ui/nodes/forward.h"
#include "util/optional.h"

namespace graphics {
    class UIManager;
}

namespace graphics::ui {

    template <typename T>
    concept UIComp = requires (T t) {
        {t.makeUINode()} -> std::same_as<std::shared_ptr<UIComponentNode>>;
    };

    template <std::derived_from<UIComponentNode> T>
    class UIComponent {
    private:
        std::shared_ptr<T> owningUINode;
        std::weak_ptr<T> uiNode;
        bool destroyOnDelete = true;
        /*std::shared_ptr<UIComponentNode> _makeNode () {
            T& derived = static_cast<T&>(*this);
            auto node = derived.makeUINode();
            uiNode = node;
            return node;
        }*/
    protected:
        std::shared_ptr<T> getNode () {
            if (!uiNode.expired()) {
                return uiNode.lock();
            } else {
                return nullptr;
            }
        }

        util::Optional<std::shared_ptr<T>> getNodeOpt () {
            if (!uiNode.expired()) {
                return uiNode.lock();
            } else {
                return util::NullOpt;
            }
        }
    public:
        explicit UIComponent(std::shared_ptr<T> owningNode) : owningUINode{std::move(owningNode)}, uiNode{owningUINode} {
            //static_assert(UIComp<T>);
        };

        ~UIComponent() {
            if (destroyOnDelete) {
                destroy();
            }
        }

        UIComponent(const UIComponent<T>&) = delete;
        UIComponent(UIComponent<T>&& other) noexcept {
            uiNode = std::move(other.uiNode);
            owningUINode = std::move(other.owningUINode);
            destroyOnDelete = other.destroyOnDelete;
            other.destroyOnDelete = false;
        }

        UIComponent<T>& operator= (const UIComponent<T>& other) = delete;
        UIComponent<T>& operator= (UIComponent<T>&& other) noexcept {
            uiNode = std::move(other.uiNode);
            owningUINode = std::move(other.owningUINode);
            destroyOnDelete = other.destroyOnDelete;
            other.destroyOnDelete = false;

            return *this;
        }

        void destroy () {
            std::shared_ptr<T> ptr;

            if ((ptr = uiNode.lock())) {
                ptr->queueDestroy();
            }


            uiNode = std::weak_ptr<T>{};
            owningUINode = nullptr;
            destroyOnDelete = false;
        }

        bool destroyed () {
            return uiNode.expired();
        }

        void detachNode () {
            destroyOnDelete = false;
        }

        UIComponent<T>& detach () {
            detachNode();
            return *this;
        }

        std::shared_ptr<UIComponentNode> transferNode () {
            // TODO: handle moving of nodes between maths
            auto oldOwningNode = std::move(owningUINode);
            owningUINode = nullptr;
            return oldOwningNode;
        }

        friend graphics::UIManager;
    };
}