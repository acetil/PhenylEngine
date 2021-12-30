#include <vector>
#include <stddef.h>

#ifndef SPAN_H
#define SPAN_H
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

        [[maybe_unused]] auto end () {
            return ptr + ptrSize;
        }
        size_t size () {
            return ptrSize;
        }

        [[maybe_unused]] size_t sizeBytes () {
            return ptrSize * sizeof(T);
        }
    };

}
#endif //SPAN_H
