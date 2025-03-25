#pragma once

#include <cstddef>
#include <memory>

#include "logging/logging.h"

namespace phenyl::graphics {
    class IUniformBuffer {
    public:
        virtual ~IUniformBuffer() = default;

        virtual unsigned char* allocate (std::size_t size) = 0;
        virtual void upload () = 0;
        virtual bool isReadable () const = 0;
    };

    template <typename T>
    class UniformBuffer {
    private:
        std::unique_ptr<IUniformBuffer> rendererBuffer;
        T* data;
    public:
        UniformBuffer() = default;

        template <typename ...Args>
        explicit UniformBuffer (std::unique_ptr<IUniformBuffer> rendererBuffer, Args&&... args) requires (std::constructible_from<T, Args&&...>) : rendererBuffer{std::move(rendererBuffer)} {
            PHENYL_DASSERT(this->rendererBuffer);
            data = reinterpret_cast<T*>(this->rendererBuffer->allocate(sizeof(T)));
            T t{std::forward<Args>(args)...};
            new (data) T(std::move(t));
        }

        explicit operator bool () const {
            return (bool)rendererBuffer;
        }

        T& operator* () {
            PHENYL_DASSERT(data);
            return *data;
        }

        const T& operator* () const {
            PHENYL_DASSERT(data);
            return *data;
        }

        T* operator-> () {
            PHENYL_DASSERT(data);
            return data;
        }

        const T* operator-> () const {
            PHENYL_DASSERT(data);
            return data;
        }

        T* get () {
            PHENYL_DASSERT(data);
            return data;
        }

        const T* get () const {
            PHENYL_DASSERT(data);
            return data;
        }

        void upload () {
            PHENYL_DASSERT(rendererBuffer);
            rendererBuffer->upload();
        }

        [[nodiscard]] bool readable () const {
            PHENYL_DASSERT(rendererBuffer);
            return rendererBuffer->isReadable();
        }

        IUniformBuffer& getUnderlying () {
            PHENYL_DASSERT(rendererBuffer);
            return *rendererBuffer;
        }

        [[nodiscard]] const IUniformBuffer& getUnderlying () const {
            PHENYL_DASSERT(rendererBuffer);
            return *rendererBuffer;
        }
    };

    class RawUniformBuffer {
    private:
        std::unique_ptr<IUniformBuffer> rendererBuffer;
        std::byte* bufData;
        std::size_t bufSize;
    public:
        RawUniformBuffer () = default;

        RawUniformBuffer (std::unique_ptr<IUniformBuffer> rendererBuffer, std::size_t size) : rendererBuffer{std::move(rendererBuffer)}, bufSize{size} {
            bufData = reinterpret_cast<std::byte*>(this->rendererBuffer->allocate(size));
        }

        explicit operator bool () const noexcept {
            return (bool)rendererBuffer;
        }

        std::byte* data () noexcept {
            return bufData;
        }

        const std::byte* data () const noexcept {
            return bufData;
        }

        std::size_t size () const noexcept {
            return bufSize;
        }

        [[nodiscard]] bool readable () const {
            PHENYL_DASSERT(rendererBuffer);
            return rendererBuffer->isReadable();
        }

        void upload () {
            PHENYL_DASSERT(rendererBuffer);
            rendererBuffer->upload();
        }

        IUniformBuffer& getUnderlying () {
            PHENYL_DASSERT(rendererBuffer);
            return *rendererBuffer;
        }

        [[nodiscard]] const IUniformBuffer& getUnderlying () const {
            PHENYL_DASSERT(rendererBuffer);
            return *rendererBuffer;
        }
    };
}