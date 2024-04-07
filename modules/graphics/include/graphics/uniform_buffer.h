#pragma once

#include <cstddef>
#include <memory>

#include "../../../logger/include/logging/logging.h"

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
}