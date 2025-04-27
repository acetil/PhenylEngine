#pragma once

#include <memory>
#include <vector>

#include "logging/logging.h"

namespace phenyl::graphics {
    enum class BufferStorageHint {
        STATIC,
        DYNAMIC
    };

    class IBuffer {
    public:
        virtual ~IBuffer() = default;

        virtual void upload (std::span<const std::byte> data) = 0;
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
        std::size_t emplace (Args&&... args) {
            auto index = size();
            data.emplace_back(std::forward<Args>(args)...);
            return index;
        }

        template <std::input_iterator It>
        std::size_t insertRange (It begin, It end) {
            auto startIndex = size();
            data.insert(data.end(), begin, end);
            return startIndex;
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
            rendererBuffer->upload(std::as_bytes(std::span{data}));
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

    class RawBuffer {
    private:
        std::unique_ptr<IBuffer> rendererBuffer;
        std::size_t bufSize = 0;
    public:
        RawBuffer () = default;
        explicit RawBuffer (std::unique_ptr<IBuffer> rendererBuffer) : rendererBuffer{std::move(rendererBuffer)} {}

        RawBuffer (const RawBuffer&) = delete;
        RawBuffer (RawBuffer&&) = default;

        RawBuffer& operator= (const RawBuffer&) = delete;
        RawBuffer& operator= (RawBuffer&&) = default;

        explicit operator bool () const noexcept {
            return static_cast<bool>(rendererBuffer);
        }

        void upload (std::span<const std::byte> data) {
            rendererBuffer->upload(data);
            this->bufSize = data.size();
        }

        std::size_t size () const noexcept {
            return bufSize;
        }

        IBuffer& getUnderlying () noexcept {
            PHENYL_DASSERT(rendererBuffer);
            return *rendererBuffer;
        }

        const IBuffer& getUnderlying () const noexcept {
            PHENYL_DASSERT(rendererBuffer);
            return *rendererBuffer;
        }
    };
}