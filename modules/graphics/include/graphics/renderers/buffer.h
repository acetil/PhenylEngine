#pragma once

#include <memory>
#include <utility>

#include "logging/logging.h"

namespace phenyl::graphics {
    class RendererBufferHandle;
    class PipelineStage;

    namespace detail {
        class BufferBase {
        private:
            std::shared_ptr<RendererBufferHandle> bufferHandle{};
        protected:
            void pushDataInt (const unsigned char* dataInternal, std::size_t size);
            void resizeInt (std::size_t newMemSize);
            void setElementSize (std::size_t elementSize);
        public:
            explicit BufferBase (std::shared_ptr<RendererBufferHandle> _bufferHandle) : bufferHandle{std::move(_bufferHandle)} {}
            ~BufferBase();

            void clearData ();

            void bufferData ();

            friend PipelineStage;
        };
    }

    // TODO: disable/change copy behaviour
    template <typename T>
    class Buffer : public detail::BufferBase {
    private:
        std::size_t elementSize{};
    public:
        Buffer () : detail::BufferBase(nullptr) {};
        explicit Buffer (std::shared_ptr<RendererBufferHandle> _bufferHandle, std::size_t _elementSize, std::size_t initialNumElements) : detail::BufferBase(std::move(_bufferHandle)),
                                                                                                                                          elementSize{_elementSize} {
            resizeBuffer(initialNumElements);
            setElementSize(elementSize * sizeof(T));
        }

        void pushData (const T* data, std::size_t num) {
            pushDataInt((unsigned char*)data, num * elementSize * sizeof(T));
        }

        template <typename DataIt>
        void pushData (DataIt begin, DataIt end) {
            static_assert(std::is_assignable_v<T, decltype(*begin)>, "Error: must be correct iterator type!");

            for (auto curr = begin; curr != end; curr++) {
                pushData(*curr);
            }
        }

        void pushData (const T& data) {
            PHENYL_DASSERT_MSG(elementSize == 1, "Pushing single data item to buffer when element size is not 1!");
            pushDataInt((const unsigned char*)&data, sizeof(T));
        }

        void resizeBuffer (std::size_t newNumVertices) {
            resizeInt(newNumVertices * elementSize * sizeof(T));
        }
    };
}