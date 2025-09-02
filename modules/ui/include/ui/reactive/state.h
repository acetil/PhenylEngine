#pragma once

#include "logging/logging.h"

#include <memory>

namespace phenyl::graphics {
class IUIObservable;

class IUIDirtyable {
public:
    virtual ~IUIDirtyable () = default;

    virtual void markDirty () = 0;

    virtual void addObservable (IUIObservable& observable) = 0;
};

class IUIObservable {
public:
    virtual ~IUIObservable () = default;

    virtual void removeListener (IUIDirtyable& dirtyable) = 0;
};

namespace detail {
    template <typename T>
    struct UIStateHolder {
        IUIDirtyable& dirtyable;
        T state;
    };
} // namespace detail

template <typename T>
class UIState {
public:
    UIState () = default;

    const T& operator* () const noexcept {
        PHENYL_DASSERT(m_state);
        return m_state->state;
    }

    const T* operator->() const noexcept {
        PHENYL_DASSERT(m_state);
        return &m_state->state;
    }

    void set (T&& newState) const {
        PHENYL_DASSERT(m_state);
        m_state->state = std::move(newState);
        m_state->dirtyable.markDirty();
    }

    template <std::invocable<T&> F>
    void update (F&& func) const {
        PHENYL_DASSERT(m_state);
        std::forward<F>(func)(m_state->state);
        m_state->dirtyable.markDirty();
    }

private:
    explicit UIState (IUIDirtyable& dirtyable, T&& initial) :
        m_state{std::make_unique<detail::UIStateHolder<T>>(dirtyable, std::move(initial))} {}

    std::unique_ptr<detail::UIStateHolder<T>> m_state;

    friend class UIComponentBase;
};
} // namespace phenyl::graphics
