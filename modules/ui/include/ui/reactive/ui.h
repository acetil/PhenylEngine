#pragma once

#include "component.h"
#include "node.h"

namespace phenyl::graphics {
class UIRoot;

class UI : public core::IResource {
public:
    template <typename T, typename Props>
    requires UIComponentType<T, Props>
    void render (Props props) {
        render<T>(currentSize(), std::move(props));
    }

    template <typename T, typename Props>
    requires UIComponentType<T, Props>
    void render (std::size_t key, Props props) {
        UIComponentBase* comp = construct<T>(key, std::move(props));
        pushComp(key);
        comp->render(*this);
        pop();
    }

    template <UIComponentType<void> T>
    void render () {
        render<T>(currentSize());
    }

    template <UIComponentType<void> T>
    void render (std::size_t key) {
        UIComponentBase* comp = construct<T>(key);
        pushComp(key);
        comp->render(*this);
        pop();
    }

    template <std::invocable<UI&> F>
    void renderChildren (F&& func) {
        std::forward<F>(func)(*this);
    }

    template <std::derived_from<UINode> T, typename... Args>
    UINode& constructNode (Args&&... args) {
        return addNode(std::make_unique<T>(std::forward<Args>(args)...));
    }

    virtual void markDirty () = 0;

    virtual UIRoot& root () = 0;

    std::string_view getName () const noexcept override;

protected:
    virtual UIComponentBase* current (std::size_t key) = 0;
    [[nodiscard]] virtual std::size_t currentSize () const = 0;
    virtual UIComponentBase* setCurrent (std::size_t key, std::unique_ptr<UIComponentBase> component) = 0;
    virtual UINode& addNode (std::unique_ptr<UINode> node) = 0;

    virtual void pushComp (std::size_t key) = 0;
    virtual void pop () = 0;

private:
    template <typename T, typename Props>
    requires UIComponentType<T, std::remove_cvref_t<Props>>
    UIComponentBase* construct (std::size_t key, Props&& props) {
        auto compType = meta::TypeIndex::Get<T>();
        auto* comp = current(key);
        if (comp && comp->type() == compType) {
            reinterpret_cast<T*>(comp)->setProps(std::forward<Props>(props));
            return comp;
        }

        return setCurrent(key, std::make_unique<T>(*this, std::forward<Props>(props)));
    }

    template <UIComponentType<void> T>
    UIComponentBase* construct (std::size_t key) {
        auto compType = meta::TypeIndex::Get<T>();
        auto* comp = current(key);
        if (comp && comp->type() == compType) {
            return comp;
        }

        return setCurrent(key, std::make_unique<T>(*this));
    }
};
} // namespace phenyl::graphics
