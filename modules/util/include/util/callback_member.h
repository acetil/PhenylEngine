#pragma once

#include <type_traits>
#include <utility>

#include <stdio.h>

namespace util {
    template <typename T, typename C>
    class CallbackMember {
    public:
        using SetCallback_t = void (C::*) (T&&);
        using GetCallback_t = const T& (C::*) ();
    private:
        SetCallback_t setCallback;
        GetCallback_t getCallback;
        C& classRef;
    public:
        CallbackMember (C& _classRef, GetCallback_t _getCallback, SetCallback_t _setCallback) : classRef{_classRef}, setCallback{_setCallback}, getCallback{_getCallback} {}

        CallbackMember& operator= (T&& obj) {
            std::invoke(setCallback, classRef, std::forward<T&&>(obj));
            return *this;
        }

        operator T () {
            return std::invoke(getCallback, classRef);
        }
    };
}