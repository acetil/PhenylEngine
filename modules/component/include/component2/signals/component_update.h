#pragma once

#include <cstddef>

namespace phenyl::component {
    template <typename T>
    struct OnInsert2 {
    private:
        T* ptr;
    public:
        explicit OnInsert2 (std::byte* ptr) : ptr{static_cast<T*>(ptr)} {}

        T& get () const noexcept {
            return *ptr;
        }
    };

    template <typename T>
    struct OnRemove2 {
    private:
        T* ptr;
    public:
        explicit OnRemove2 (std::byte* ptr) : ptr{static_cast<T*>(ptr)} {}

        T& get () const noexcept {
            return *ptr;
        }
    };
}