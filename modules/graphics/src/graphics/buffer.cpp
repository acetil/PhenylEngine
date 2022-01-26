#include "graphics/buffer.h"

using namespace graphics;

Buffer::Buffer () {
    ownsMemory = false;
    memory = nullptr;
    elementSize = 0;
    numElements = 0;
    maxNumElements = 0;
    isStatic = false;
}

void* Buffer::getData () {
    return memory;
}


void Buffer::clearData () {
    numElements = 0;
}

Buffer::Buffer (int maxNumElements, int elementSize, bool isStatic) {
    static_assert(sizeof(char) == 1, "Char has a greater than 1 size on this system!");
    memory = (void*) new char[maxNumElements * elementSize];
    this->maxNumElements = maxNumElements;
    this->elementSize = elementSize;
    this->isStatic = isStatic;
    this->numElements = 0;
    ownsMemory = true;
}

Buffer::~Buffer () {
    if (ownsMemory) {
        delete[] (char*)memory;
    }
}
Buffer::Buffer (const Buffer& copy) {
    this->maxNumElements = copy.maxNumElements;
    this->elementSize = copy.elementSize;
    this->isStatic = copy.isStatic;
    this->numElements = copy.numElements;
    this->memory = copy.memory;
    ownsMemory = false; // copy constructor does not move memory ownership
}
Buffer::Buffer(Buffer &&move)  noexcept {
    this->maxNumElements = move.maxNumElements;
    this->elementSize = move.elementSize;
    this->isStatic = move.isStatic;
    this->numElements = move.numElements;
    this->memory = move.memory;
    ownsMemory = true;
    move.ownsMemory = false; // moves memory ownership
}

Buffer& Buffer::operator=(Buffer &&move) noexcept {
    if (this != &move) {
        if (ownsMemory) {
            delete[] (char*)memory;
        }
        memory = move.memory;
        this->maxNumElements = move.maxNumElements;
        this->elementSize = move.elementSize;
        this->isStatic = move.isStatic;
        this->numElements = move.numElements;
        ownsMemory = move.ownsMemory;
        move.ownsMemory = false;
    }
    return *this;
}
