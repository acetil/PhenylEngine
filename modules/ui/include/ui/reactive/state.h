#pragma once

#include "logging/logging.h"

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

template <typename T>
class UIState {
public:
    const T& operator* () const noexcept {
        return m_state;
    }

    const T* operator->() const noexcept {
        return &m_state;
    }

    template <std::invocable<T&> F>
    void set (F&& func) {
        std::forward<F>(func)(m_state);
        m_dirtyable.markDirty();
    }

private:
    explicit UIState (IUIDirtyable& dirtyable, T&& state) : m_dirtyable{dirtyable}, m_state{std::forward<T>(state)} {}

    IUIDirtyable& m_dirtyable;
    T m_state;

    friend class UIComponentBase;
};
} // namespace phenyl::graphics
