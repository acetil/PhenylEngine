#include "component/detail/component_vector.h"

using namespace phenyl::component;

UntypedComponentVector::UntypedComponentVector (std::size_t typeIndex, std::size_t dataSize, std::size_t startCapacity) : typeIndex{typeIndex}, compSize{dataSize}, vecLength{0}, vecCapacity{startCapacity} {
    memory = std::make_unique<std::byte[]>(dataSize * startCapacity);
}

UntypedComponentVector::UntypedComponentVector (UntypedComponentVector&& other) noexcept : typeIndex{other.typeIndex}, memory{std::move(other.memory)}, compSize{other.compSize}, vecLength{other.vecLength}, vecCapacity{other.vecCapacity} {
    other.compSize = 0;
    other.vecLength = 0;
    other.vecCapacity = 0;
}

UntypedComponentVector& UntypedComponentVector::operator= (UntypedComponentVector&& other) noexcept {
    PHENYL_DASSERT(typeIndex == other.typeIndex);
    if (memory) {
        deleteAllComps(memory.get(), memory.get() + compSize * vecCapacity);
    }

    memory = std::move(other.memory);
    compSize = other.compSize;
    vecLength = other.vecLength;
    vecCapacity = other.vecCapacity;
    return *this;
}

std::byte* UntypedComponentVector::insertUntyped () {
    guaranteeLength(vecLength + 1);
    PHENYL_DASSERT(vecCapacity >= vecLength + 1);

    return memory.get() + (vecLength++) * compSize;
}

void UntypedComponentVector::moveFrom (UntypedComponentVector& other, std::size_t pos) {
    PHENYL_DASSERT(type() == other.type());

    auto* ptr = insertUntyped();
    moveConstructComp(other.getUntyped(pos), ptr);
}

void UntypedComponentVector::remove (std::size_t pos) {
    PHENYL_DASSERT(pos < size());

    if (pos == size() - 1) {
        // Delete from back
        deleteComp(getUntyped(pos));
    } else {
        // Swap from back
        auto oldPos = size() - 1;
        moveComp(getUntyped(oldPos), getUntyped(pos));
        deleteComp(getUntyped(oldPos));
    }
    vecLength--;
}

void UntypedComponentVector::clear() {
    deleteAllComps(memory.get(), memory.get() + vecLength * compSize);
    vecLength = 0;
}

void UntypedComponentVector::guaranteeLength (std::size_t newLen) {
    if (newLen <= vecCapacity) {
        return;
    }

    std::size_t newCapacity = vecCapacity * RESIZE_FACTOR;
    std::unique_ptr<std::byte[]> newMemory = std::make_unique<std::byte[]>(newCapacity * compSize);
    moveAllComps(memory.get(), memory.get() + vecCapacity * compSize, newMemory.get());

    vecCapacity = newCapacity;
    memory = std::move(newMemory);
}
