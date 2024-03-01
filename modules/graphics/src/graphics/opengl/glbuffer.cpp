#include <cstring>

#include "glbuffer.h"

using namespace phenyl::graphics;

GlBuffer::GlBuffer () {
    glGenBuffers(1, &bufferId);
}


void GlBuffer::allocBufferMemory (std::size_t memSize) {
    bindBuffer();
    glBufferData(GL_ARRAY_BUFFER, memSize, nullptr, GL_DYNAMIC_DRAW);

    data = std::make_unique<unsigned char[]>(memSize);
    maxSize = memSize;
}

void GlBuffer::bindBuffer () const {
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
}

void GlBuffer::bufferItems (const void* _data, std::size_t size) {
    PHENYL_DASSERT_MSG(currentSize + size <= maxSize, "Buffer to GlBuffer failed: max size of {} exceeded!", maxSize);

    std::memcpy(data.get() + currentSize, _data, size);
    currentSize += size;
}

void GlBuffer::bufferData () {
    bindBuffer();
    glBufferData(GL_ARRAY_BUFFER, currentSize, data.get(), GL_DYNAMIC_DRAW);
}

void GlBuffer::clearBuffer () {
    currentSize = 0;
}

std::size_t GlBuffer::getNumElements () const{
    return currentSize / elementSize;
}

GlBuffer::~GlBuffer () {
    glDeleteBuffers(1, &bufferId);
}

void GlBuffer::setElementSize (std::size_t elementSize) {
    this->elementSize = elementSize;
}
