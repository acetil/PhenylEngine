#pragma once

#include <cstddef>

namespace phenyl::core {
template<typename T>
struct OnInsert {
public:
    explicit OnInsert (std::byte* ptr) : m_ptr{reinterpret_cast<T*>(ptr)} {}

    T& get () const noexcept {
        return *m_ptr;
    }

private:
    T* m_ptr;
};

template<typename T>
struct OnRemove {
public:
    explicit OnRemove (std::byte* ptr) : m_ptr{reinterpret_cast<T*>(ptr)} {}

    T& get () const noexcept {
        return *m_ptr;
    }

private:
    T* m_ptr;
};
} // namespace phenyl::core
