#pragma once

#include <memory>
#include <vector>

#include "logging/logging.h"

namespace phenyl::graphics {
    class IBuffer {
    public:
        virtual ~IBuffer() = default;

        virtual void upload (unsigned char* data, std::size_t size) = 0;
    };

    template <typename T>
    class Buffer {
    private:
        std::unique_ptr<IBuffer> rendererBuffer;
        std::vector<T> data;
    public:
        Buffer () : rendererBuffer{}, data{} {}
        explicit Buffer (std::unique_ptr<IBuffer> rendererBuffer) : rendererBuffer{std::move(rendererBuffer)}, data{} {}

        explicit operator bool () const {
            return (bool)rendererBuffer;
        }

        template <typename ...Args>
        void emplace (Args&&... args) {
            data.emplace_back(std::forward<Args>(args)...);
        }

        template <std::input_iterator It>
        void insertRange (It begin, It end) {
            data.insert(data.end(), begin, end);
        }

        void reserve (std::size_t newSize) {
            data.reserve(newSize);
        }

        std::size_t size () const {
            return data.size();
        }

        void clear () {
            data.clear();
        }

        void upload () {
            rendererBuffer->upload(reinterpret_cast<unsigned char*>(data.data()), data.size() * sizeof(T));
        }

        IBuffer& getUnderlying () {
            PHENYL_DASSERT(rendererBuffer);
            return *rendererBuffer;
        }

        const IBuffer& getUnderlying () const {
            PHENYL_DASSERT(rendererBuffer);
            return *rendererBuffer;
        }
    };
}