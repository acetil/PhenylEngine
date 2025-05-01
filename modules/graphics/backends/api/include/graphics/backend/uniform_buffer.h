#pragma once

#include <cstddef>
#include <memory>

#include "logging/logging.h"

namespace phenyl::graphics {
    class IUniformBuffer {
    public:
        virtual ~IUniformBuffer() = default;

        virtual std::span<std::byte> allocate (std::size_t size) = 0;
        virtual void upload () = 0;
        virtual bool isReadable () const = 0;
        virtual std::size_t getMinAlignment () const noexcept = 0;
    };

    template <typename T>
    class UniformBuffer {
    private:
        std::unique_ptr<IUniformBuffer> m_buffer;
        T* m_data;
    public:
        UniformBuffer() = default;

        template <typename ...Args>
        explicit UniformBuffer (std::unique_ptr<IUniformBuffer> rendererBuffer, Args&&... args) requires (std::constructible_from<T, Args&&...>) : m_buffer{std::move(rendererBuffer)} {
            PHENYL_DASSERT(this->m_buffer);
            m_data = reinterpret_cast<T*>(this->m_buffer->allocate(sizeof(T)).data());
            T t{std::forward<Args>(args)...};
            new (m_data) T(std::move(t));
        }

        explicit operator bool () const {
            return static_cast<bool>(m_buffer);
        }

        T& operator* () {
            PHENYL_DASSERT(m_data);
            return *m_data;
        }

        const T& operator* () const {
            PHENYL_DASSERT(m_data);
            return *m_data;
        }

        T* operator-> () {
            PHENYL_DASSERT(m_data);
            return m_data;
        }

        const T* operator-> () const {
            PHENYL_DASSERT(m_data);
            return m_data;
        }

        T* get () {
            PHENYL_DASSERT(m_data);
            return m_data;
        }

        const T* get () const {
            PHENYL_DASSERT(m_data);
            return m_data;
        }

        void upload () {
            PHENYL_DASSERT(m_buffer);
            m_buffer->upload();
        }

        [[nodiscard]] bool readable () const {
            PHENYL_DASSERT(m_buffer);
            return m_buffer->isReadable();
        }

        IUniformBuffer& getUnderlying () {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }

        [[nodiscard]] const IUniformBuffer& getUnderlying () const {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }
    };

    template <typename T>
    class UniformArrayBuffer {
    private:
        std::unique_ptr<IUniformBuffer> m_buffer;
        std::byte* m_data;
        std::size_t m_stride = 0;
        std::size_t m_size = 0;
        std::size_t m_capacity = 0;

        void guaranteeCapacity (std::size_t reqCapacity) {
            if (m_capacity >= reqCapacity) {
                return;
            }

            while (m_capacity < reqCapacity) {
                m_capacity *= 2;
            }

            m_data = this->m_buffer->allocate(m_stride * m_capacity).data();
        }

        T* get (std::size_t index) {
            return reinterpret_cast<T*>(m_data + index * m_stride);
        }
    public:
        UniformArrayBuffer () = default;
        UniformArrayBuffer (std::unique_ptr<IUniformBuffer> rendererBuffer, std::size_t startCapacity = 8) : m_buffer{std::move(rendererBuffer)}, m_capacity{startCapacity} {
            PHENYL_DASSERT(this->m_buffer);

            // https://blog.xoria.org/rounding-up/
            auto strideMask = this->m_buffer->getMinAlignment() - 1;
            m_stride = (sizeof(T) + strideMask) & ~strideMask;

            m_data = this->m_buffer->allocate(m_stride * m_capacity).data();
        }

        explicit operator bool () const {
            return static_cast<bool>(m_buffer);
        }

        std::size_t size () const noexcept {
            return m_size;
        }

        std::size_t capacity () const noexcept {
            return m_capacity;
        }

        void reserve (std::size_t capacity) {
            guaranteeCapacity(capacity);
        }

        T& push (T&& obj) {
            guaranteeCapacity(m_size + 1);

            PHENYL_DASSERT(m_data);
            new (get(m_size)) T(std::forward<T>(obj));

            return *get(m_size++);
        }

        T& at (std::size_t pos) {
            PHENYL_ASSERT(pos < m_size);
            PHENYL_DASSERT(m_data);
            return *get(pos);
        }

        T& operator[] (std::size_t pos) {
            PHENYL_DASSERT(pos < m_size);
            PHENYL_DASSERT(m_data);
            return *get(pos);
        }

        void clear () {
            m_size = 0;
        }

        void upload () {
            PHENYL_DASSERT(m_buffer);
            m_buffer->upload();
        }

        IUniformBuffer& getUnderlying () {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }

        [[nodiscard]] const IUniformBuffer& getUnderlying () const {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }

        std::size_t stride () const noexcept {
            return m_stride;
        }
    };

    class RawUniformBuffer {
    private:
        std::unique_ptr<IUniformBuffer> m_buffer;
        std::span<std::byte> m_data;
    public:
        RawUniformBuffer () = default;

        RawUniformBuffer (std::unique_ptr<IUniformBuffer> rendererBuffer, std::size_t size) : m_buffer{std::move(rendererBuffer)} {
            m_data = this->m_buffer->allocate(size);
        }

        explicit operator bool () const noexcept {
            return static_cast<bool>(m_buffer);
        }

        std::byte* data () noexcept {
            return m_data.data();
        }

        const std::byte* data () const noexcept {
            return m_data.data();
        }

        std::size_t size () const noexcept {
            return m_data.size();
        }

        [[nodiscard]] bool readable () const {
            PHENYL_DASSERT(m_buffer);
            return m_buffer->isReadable();
        }

        void upload () {
            PHENYL_DASSERT(m_buffer);
            m_buffer->upload();
        }

        IUniformBuffer& getUnderlying () {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }

        [[nodiscard]] const IUniformBuffer& getUnderlying () const {
            PHENYL_DASSERT(m_buffer);
            return *m_buffer;
        }
    };
}