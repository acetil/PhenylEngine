#pragma once

#include "state.h"

#include <memory>
#include <unordered_set>

namespace phenyl::graphics {
namespace detail {
    template <typename T>
    class UIAtomState : public IUIObservable {
    public:
        template <typename... Args>
        explicit UIAtomState(Args&&... args) : m_obj{std::forward<Args>(args)...} {}

        const T& get () const noexcept {
            return m_obj;
        }

        void set (T&& obj) {
            m_obj = std::move(obj);
            onUpdate();
        }

        template <std::invocable<T&> F>
        void update (F&& fn) {
            std::forward<F>(fn)(m_obj);
            onUpdate();
        }

        void addListener (IUIDirtyable& listener) {
            m_listeners.emplace(&listener);
        }

        void removeListener (IUIDirtyable& listener) override {
            m_listeners.erase(&listener);
        }

    private:
        T m_obj;
        std::unordered_set<IUIDirtyable*> m_listeners;

        void onUpdate () {
            for (auto* listener : m_listeners) {
                listener->markDirty();
            }
        }
    };
} // namespace detail

template <typename T>
class UIAtom {
public:
    template <typename... Args>
    static UIAtom Make (Args&&... args) {
        return UIAtom{std::make_shared<detail::UIAtomState<T>>(std::forward<Args>(args)...)};
    }

    UIAtom () = default;

    explicit operator bool () const noexcept {
        return static_cast<bool>(m_data);
    }

    const T& operator* () const noexcept {
        return m_data->get();
    }

    const T* operator->() const noexcept {
        return &m_data->get();
    }

    void set (T&& obj) const {
        m_data->set(std::forward<T>(obj));
    }

    template <std::invocable<T&> F>
    void update (F&& fn) const {
        m_data->update(std::forward<F>(fn));
    }

    void addUsing (IUIDirtyable& listener) const {
        m_data->addListener(listener);
        listener.addObservable(*m_data);
    }

private:
    explicit UIAtom (std::shared_ptr<detail::UIAtomState<T>> data) : m_data{data} {}

    std::shared_ptr<detail::UIAtomState<T>> m_data = nullptr;
};
} // namespace phenyl::graphics
