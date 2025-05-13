#pragma once

#include <cstddef>
#include <memory>

#ifndef NDEBUG
    #include <stdexcept>
#endif

#include "logging/logging.h"

namespace phenyl::util {
template<typename T>
class FixedStack {
public:
    FixedStack (std::size_t _size) : m_capacity{_size}, m_data{std::make_unique<T[]>(_size)} {}

    void push (T&& obj) {
        if (m_size >= m_capacity) {
            logging::log(LEVEL_ERROR, "Attempted to push to full stack!");
#ifndef NDEBUG
            throw std::out_of_range("Attempted to push to full stack!");
#endif
        }
        m_data[m_size++] = std::move(obj);
    }

    T pop () {
        if (m_size == 0) {
            logging::log(LEVEL_ERROR, "Attempted to pop from empty stack!");
#ifndef NDEBUG
            throw std::out_of_range("Attempted to pop from empty stack!");
#else
            return std::move(data[0]);
#endif
        }
        return std::move(m_data[--m_size]);
    }

    bool isEmpty () {
        return m_size == 0;
    }

    bool isFull () {
        return m_size >= m_capacity;
    }

    std::size_t size () {
        return m_size;
    }

    std::size_t capacity () {
        return m_capacity;
    }

private:
    std::size_t m_capacity;
    std::size_t m_size{};
    std::unique_ptr<T[]> m_data;
};
} // namespace phenyl::util
