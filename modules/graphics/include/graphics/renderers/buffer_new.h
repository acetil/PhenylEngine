#pragma once

#include <memory>
#include <utility>

#include "logging/logging.h"

namespace graphics {
    class RendererBufferHandle;
    class PipelineStage;

    /*namespace detail {
        class BufferNewBaseA {
        private:
            std::shared_ptr<RendererBufferHandle> bufferHandle{};
        protected:
            std::size_t numElements{};
            void allocBufferMemory (std::size_t memSize);
            //void* reallocBufferMemory (std::size_t newSize);

            void bufInternal ();
            void bufDataInternal (void* data, std::size_t size);
            void resetBufferInternal ();
        public:
            BufferNewBaseA() = default;
            explicit BufferNewBaseA (std::shared_ptr<RendererBufferHandle> _bufferHandle) : bufferHandle{std::move(_bufferHandle)} {}
            ~BufferNewBaseA() = default;

            friend PipelineStage;
        };
    }


    template <typename T>
    class BufferNewA : public detail::BufferNewBaseA {
    private:
        //std::unique_ptr<T[]> bufferMemory;
        //std::size_t numElements{};
        std::size_t bufferCapacity{};
        std::size_t elementSize{};
    public:
        BufferNewA() = default;
        explicit BufferNewA (std::shared_ptr<RendererBufferHandle> bufferHandle, unsigned int bufCapacity, unsigned int _elementSize) : detail::BufferNewBase(std::move(bufferHandle)),
                bufferCapacity{bufCapacity}, elementSize{_elementSize} {
            //bufferMemory = std::unique_ptr<T[]>((T*)allocBufferMemory(bufferCapacity * elementSize * sizeof(T)));
            allocBufferMemory(bufCapacity*elementSize*sizeof(T));
        };

        //BufferNew(BufferNew&) = delete;
        //BufferNew operator= (BufferNew&) = delete;

        void pushData (T* data, std::size_t num) {
            if (numElements + num * elementSize > bufferCapacity) {
                logging::log(LEVEL_WARNING, "Buffer overrun!");
            } else {
                bufDataInternal(data, num * elementSize * sizeof(T));
            }
        };

        void clearData () {
            //numElements = 0;
            resetBufferInternal();
        };

        void bufferData () {
            bufInternal();
        }
    };*/

    namespace detail {
        class BufferNewBase {
        private:
            std::shared_ptr<RendererBufferHandle> bufferHandle{};
        protected:
            void pushDataInt (unsigned char* dataInternal, std::size_t size);
            void resizeInt (std::size_t newMemSize);
            void setElementSize (std::size_t elementSize);
        public:
            explicit BufferNewBase (std::shared_ptr<RendererBufferHandle> _bufferHandle) : bufferHandle{std::move(_bufferHandle)} {}
            ~BufferNewBase();

            void clearData ();

            void bufferData ();

            friend PipelineStage;
        };
    }

    template <typename T>
    class BufferNew : public detail::BufferNewBase {
    private:
        std::size_t elementSize{};
    public:
        BufferNew () : detail::BufferNewBase(nullptr) {};
        explicit BufferNew (std::shared_ptr<RendererBufferHandle> _bufferHandle, std::size_t _elementSize, std::size_t initialNumElements) : detail::BufferNewBase(std::move(_bufferHandle)),
                elementSize{_elementSize} {
            resizeBuffer(initialNumElements);
            setElementSize(elementSize * sizeof(T));
        }

        void pushData (T* data, std::size_t num) {
            pushDataInt((unsigned char*)data, num * elementSize * sizeof(T));
        }

        void pushData (T& data) {
#ifndef NDEBUG
            if (elementSize > 1) {
                logging::log(LEVEL_WARNING, "Pushing single data item to buffer when element size is not 1!");
                return;
            }
#endif
            pushDataInt(&data, sizeof(T));
        }

        void resizeBuffer (std::size_t newNumVertices) {
            resizeInt(newNumVertices * elementSize * sizeof(T));
        }
    };
}