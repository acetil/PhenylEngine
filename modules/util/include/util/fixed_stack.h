#pragma once

#include <cstddef>
#include <memory>

#ifndef NDEBUG
#include <stdexcept>
#endif

#include "logging/logging.h"

namespace util {
    template <typename T>
    class FixedStack {
    private:
        std::size_t maxSize;
        std::size_t num{};
        std::unique_ptr<T[]> data;
    public:
        FixedStack (std::size_t _size) : maxSize{_size}, data{std::make_unique<T[]>(_size)} {}

        void push (T&& obj) {
            if (num >= maxSize) {
                logging::log(LEVEL_ERROR, "Attempted to push to full stack!");
#ifndef NDEBUG
                throw std::out_of_range("Attempted to push to full stack!");
#endif
            }
            data[num++] = std::move(obj);
        }

        T pop () {
            if (num == 0) {
                logging::log(LEVEL_ERROR, "Attempted to pop from empty stack!");
#ifndef NDEBUG
                throw std::out_of_range("Attempted to pop from empty stack!");
#else
                return std::move(data[0]);
#endif
            }
            return std::move(data[--num]);
        }

        bool isEmpty () {
            return num == 0;
        }

        bool isFull () {
            return num >= maxSize;
        }

        std::size_t size () {
            return num;
        }

        std::size_t capacity () {
            return maxSize;
        }
    };
}