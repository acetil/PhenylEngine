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
        std::unique_ptr<IBuffer> m_buffer;
        std::vector<T> m_data;
    public:
        Buffer () : m_buffer{}, m_data{} {}
        explicit Buffer (std::unique_ptr<IBuffer> rendererBuffer) : m_buffer{std::move(rendererBuffer)}, m_data{} {}

        explicit operator bool () const {
            return (bool)m_buffer;
        }

        template <typename ...Args>
        std::size_t emplace (Args&&... args) {
            auto index = size();
            m_data.emplace_back(std::forward<Args>(args)...);
            return index;
        }

        template <std::input_iterator It>
        std::size_t insertRange (It begin, It end) {
            auto startIndex = size();
            m_data.insert(m_data.end(), begin, end);
            return startIndex;
        }

        void reserve (std::size_t newSize) {
            m_data.reserve(newSize);
        }

        std::size_t size () const {
            return m_data.size();
        }

        void clear () {
            m_data.clear();
        }

        void upload () {
            m_buffer->upload(std::as_bytes(std::span{m_data}));
        }

        IBuffer& getUnderlying () {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }

        const IBuffer& getUnderlying () const {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }
    };

    class RawBuffer {
    private:
        std::unique_ptr<IBuffer> m_buffer;
        std::size_t m_size = 0;
    public:
        RawBuffer () = default;
        explicit RawBuffer (std::unique_ptr<IBuffer> rendererBuffer) : m_buffer{std::move(rendererBuffer)} {}

        RawBuffer (const RawBuffer&) = delete;
        RawBuffer (RawBuffer&&) = default;

        RawBuffer& operator= (const RawBuffer&) = delete;
        RawBuffer& operator= (RawBuffer&&) = default;

        explicit operator bool () const noexcept {
            return static_cast<bool>(m_buffer);
        }

        void upload (std::span<const std::byte> data) {
            m_buffer->upload(data);
            this->m_size = data.size();
        }

        std::size_t size () const noexcept {
            return m_size;
        }

        IBuffer& getUnderlying () noexcept {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }

        const IBuffer& getUnderlying () const noexcept {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }
    };
}