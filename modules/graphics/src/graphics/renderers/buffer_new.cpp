#include "graphics/renderers/buffer_new.h"
#include "graphics/renderers/renderer_buffer.h"

using namespace graphics;

/*void graphics::detail::BufferNewBase::allocBufferMemory (std::size_t memSize) {
    bufferHandle->allocBufferMemory(memSize);
}

void detail::BufferNewBase::bufInternal () {
    bufferHandle->bufferData();
}

void detail::BufferNewBase::bufDataInternal (void* data, std::size_t size) {
    bufferHandle->bufferItems(data, size);
    numElements += size;
}

void detail::BufferNewBase::resetBufferInternal () {
    bufferHandle->clearBuffer();
    numElements = 0;
}*/

detail::BufferNewBase::~BufferNewBase () = default;

void detail::BufferNewBase::clearData () {
    bufferHandle->clearBuffer();
}

void detail::BufferNewBase::bufferData () {
    bufferHandle->bufferData();
}

void detail::BufferNewBase::pushDataInt (unsigned char* dataInternal, std::size_t size) {
    bufferHandle->bufferItems(dataInternal, size);
}

void detail::BufferNewBase::resizeInt (std::size_t newMemSize) {
    bufferHandle->allocBufferMemory(newMemSize);
}

void detail::BufferNewBase::setElementSize (std::size_t elementSize) {
    bufferHandle->setElementSize(elementSize);
}
