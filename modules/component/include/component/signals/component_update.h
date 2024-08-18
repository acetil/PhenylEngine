#pragma once

#include <cstddef>

namespace phenyl::component {
    template <typename T>
    struct OnInsert {
    private:
        T* ptr;
    public:
        explicit OnInsert (std::byte* ptr) : ptr{reinterpret_cast<T*>(ptr)} {}

        T& get () const noexcept {
            return *ptr;
        }
    };

    template <typename T>
    struct OnRemove {
    private:
        T* ptr;
    public:
        explicit OnRemove (std::byte* ptr) : ptr{reinterpret_cast<T*>(ptr)} {}

        T& get () const noexcept {
            return *ptr;
        }
    };
}