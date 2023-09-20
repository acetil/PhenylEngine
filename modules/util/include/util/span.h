#pragma once

#include <vector>
#include <stddef.h>

namespace phenyl::util {
    template<typename T>
    class span {
    private:
        T* ptr;
        size_t ptrSize;
    public:
        span () : ptr{nullptr}, ptrSize{0} {};
        span (T* data, size_t s) : ptr{data}, ptrSize{s} {};
        span (T* begin, T* end) : ptr{begin}, ptrSize{(size_t)(end - begin)} {};
        T* begin() {
            return ptr;
        }

        const T* begin() const {
            return cbegin();
        }
        const T* cbegin () const {
            return ptr;
        }

        T* end () {
            return ptr + ptrSize;
        }
        const T* end () const {
            return cend();
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
