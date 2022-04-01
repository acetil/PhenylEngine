#pragma once

#include <vector>
#include <stddef.h>

namespace util {
    template<typename T>
    class span {
    private:
        T* ptr;
        size_t ptrSize;
    public:
        span () : ptr{nullptr}, ptrSize{0} {};
        span (T* data, size_t s) : ptr{data}, ptrSize{s} {};
        span (T* begin, T* end) : ptr{begin}, ptrSize{(size_t)(end - begin)} {};
        auto begin() {
            return ptr;
        };

        const T* cbegin () const {
            return ptr;
        }

        [[maybe_unused]] auto end () {
            return ptr + ptrSize;
        }

        const T* cend () const {
            return ptr + ptrSize;
        }
        [[nodiscard]] size_t size () const {
            return ptrSize;
        }

        [[maybe_unused]] [[nodiscard]] size_t sizeBytes () const {
            return ptrSize * sizeof(T);
        }
    };

}
