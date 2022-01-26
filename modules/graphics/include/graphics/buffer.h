#pragma once

#include <string.h>
#include "logging/logging.h"

namespace graphics {
    class Buffer {
    private:
        bool ownsMemory;
        void* memory;
        int elementSize;
        int numElements;
        int maxNumElements;
        bool isStatic;
    public:
        Buffer ();
        Buffer (int maxNumElements, int elementSize, bool isStatic);

        ~Buffer ();

        Buffer (const Buffer &copy); // copy constructor, does NOT move memory ownership
        Buffer (Buffer &&move) noexcept; // move constructor, DOES move memory ownership
        Buffer& operator= (Buffer && move) noexcept;
        template<typename T>
        void pushData (T* data, int num) {
            if (sizeof(T) != elementSize) {
                logging::log(LEVEL_WARNING,
                              "Attempted to push data with element size {}, but buffer element size is {}!",
                              sizeof(T), elementSize);
                return;
            }
            if (numElements >= maxNumElements) {
                logging::log(LEVEL_WARNING, "Attempted to push an element to buffer when buffer was at capacity!");
                return;
            }
            memcpy(((T*) memory) + numElements, data, num * sizeof(T));
            numElements += num;
        }

        [[maybe_unused]] [[nodiscard]]
        bool isEmpty () const {
            return maxNumElements == 0;
        }
        [[nodiscard]]
        int currentSize () const {
            return elementSize * numElements;
        };
        void* getData ();
        void clearData ();
    };

}

