#pragma once

#include <stdio.h>
#include <type_traits>
#include <utility>

namespace phenyl::util {
template<typename T, typename C>
class CallbackMember {
public:
    using SetCallback_t = void (C::*)(T&&);
    using GetCallback_t = const T& (C::*) ();

    CallbackMember (C& _classRef, GetCallback_t _getCallback, SetCallback_t _setCallback) :
        m_classRef{&_classRef},
        m_setCallback{_setCallback},
        m_getCallback{_getCallback} {}

    CallbackMember& operator= (T&& obj) {
        std::invoke(m_setCallback, *m_classRef, std::forward<T&&>(obj));
        return *this;
    }

    operator T () {
        return std::invoke(m_getCallback, *m_classRef);
    }

private:
    SetCallback_t m_setCallback;
    GetCallback_t m_getCallback;
    C* m_classRef;
};
} // namespace phenyl::util
