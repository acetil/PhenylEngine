#pragma once

#include "component.h"

#include <memory>

namespace phenyl::graphics {
struct UIComponentNode;
class ConcreteUI;
class UINode;

class UIRenderResult {
public:
    class Builder {
    public:
        Builder& withChild (UIRenderResult result);
        UIRenderResult build ();

    private:
        Builder (std::unique_ptr<UINode> node);

        std::unique_ptr<UINode> m_node;

        friend class UIRenderResult;
        friend class UIContext;
    };

    std::unique_ptr<UINode> node () &&;

private:
    explicit UIRenderResult (std::unique_ptr<UINode> node);

    std::unique_ptr<UINode> m_node = nullptr;

    friend class UI;
    friend class UIContext;
    friend Builder;
};

class UIContext {
public:
    UIContext (ConcreteUI& ui, UIComponentNode* node);

    template <typename T, typename Props>
    requires UIComponentType<T, Props>
    UIRenderResult render (Props props) {
        return render<T>(childrenCount(), std::move(props));
    }

    template <typename T, typename Props>
    requires UIComponentType<T, Props>
    UIRenderResult render (std::size_t key, Props props) {
        UIComponentBase* comp = construct<T>(key, std::move(props));
        return renderChild(key, comp);
    }

    template <UIComponentType<void> T>
    UIRenderResult render () {
        return render<T>(childrenCount());
    }

    template <UIComponentType<void> T>
    UIRenderResult render (std::size_t key) {
        UIComponentBase* comp = construct<T>(key);
        return renderChild(key, comp);
    }

    template <std::derived_from<UINode> T, typename... Args>
    UIRenderResult::Builder makeNode (Args&&... args) {
        auto node = std::make_unique<T>(std::forward<Args>(args)...);
        setNode(node.get());
        return UIRenderResult::Builder{std::move(node)};
    }

    UIRenderResult makeEmptyNode () {
        return makeNode<EmptyUINode>().build();
    }

    UIRenderResult renderComponent ();

private:
    ConcreteUI& m_ui;
    UIComponentNode* m_current;

    [[nodiscard]] std::size_t childrenCount () const;
    UIComponentBase* getChild (std::size_t key) const;
    UIComponentBase* setChild (std::size_t key, std::unique_ptr<UIComponentBase> component);
    UIRenderResult renderChild (std::size_t key, UIComponentBase* component);
    void setNode (UINode* node);

    template <typename T, typename Props>
    requires UIComponentType<T, std::remove_cvref_t<Props>>
    UIComponentBase* construct (std::size_t key, Props&& props) {
        auto compType = meta::TypeIndex::Get<T>();
        auto* comp = getChild(key);
        if (comp && comp->type() == compType) {
            reinterpret_cast<T*>(comp)->setProps(std::forward<Props>(props));
            return comp;
        }

        return setChild(key, std::make_unique<T>(reinterpret_cast<UI&>(m_ui), std::forward<Props>(props)));
    }

    template <UIComponentType<void> T>
    UIComponentBase* construct (std::size_t key) {
        auto compType = meta::TypeIndex::Get<T>();
        auto* comp = getChild(key);
        if (comp && comp->type() == compType) {
            return comp;
        }

        return setChild(key, std::make_unique<T>(reinterpret_cast<UI&>(m_ui)));
    }
};

using UIComponentFactory = std::function<UIRenderResult(UIContext& ctx)>;
UIRenderResult EmptyUIFactory (UIContext& ctx);
} // namespace phenyl::graphics
