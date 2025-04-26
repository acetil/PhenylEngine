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
        virtual std::size_t getMinAlignment () const noexcept = 0;
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
            return static_cast<bool>(rendererBuffer);
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

    template <typename T>
    class UniformArrayBuffer {
    private:
        std::unique_ptr<IUniformBuffer> rendererBuffer;
        std::byte* data;
        std::size_t objStride = 0;
        std::size_t currSize = 0;
        std::size_t currCapacity = 0;

        void guaranteeCapacity (std::size_t reqCapacity) {
            if (currCapacity >= reqCapacity) {
                return;
            }

            while (currCapacity < reqCapacity) {
                currCapacity *= 2;
            }

            data =  reinterpret_cast<std::byte*>(this->rendererBuffer->allocate(objStride * currCapacity));
        }

        T* get (std::size_t index) {
            return reinterpret_cast<T*>(data + index * objStride);
        }
    public:
        UniformArrayBuffer () = default;
        UniformArrayBuffer (std::unique_ptr<IUniformBuffer> rendererBuffer, std::size_t startCapacity = 8) : rendererBuffer{std::move(rendererBuffer)}, currCapacity{startCapacity} {
            PHENYL_DASSERT(this->rendererBuffer);

            // https://blog.xoria.org/rounding-up/
            auto strideMask = this->rendererBuffer->getMinAlignment() - 1;
            objStride = (sizeof(T) + strideMask) & ~strideMask;

            data = reinterpret_cast<std::byte*>(this->rendererBuffer->allocate(objStride * currCapacity));
        }

        explicit operator bool () const {
            return static_cast<bool>(rendererBuffer);
        }

        std::size_t size () const noexcept {
            return currSize;
        }

        std::size_t capacity () const noexcept {
            return currCapacity;
        }

        void reserve (std::size_t capacity) {
            guaranteeCapacity(capacity);
        }

        T& push (T&& obj) {
            guaranteeCapacity(currSize + 1);

            PHENYL_DASSERT(data);
            new (get(currSize)) T(std::forward<T>(obj));

            return *get(currSize++);
        }

        T& at (std::size_t pos) {
            PHENYL_ASSERT(pos < currSize);
            PHENYL_DASSERT(data);
            return *get(pos);
        }

        T& operator[] (std::size_t pos) {
            PHENYL_DASSERT(pos < currSize);
            PHENYL_DASSERT(data);
            return *get(pos);
        }

        void clear () {
            currSize = 0;
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

        std::size_t stride () const noexcept {
            return objStride;
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
            return static_cast<bool>(rendererBuffer);
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