#include "core/component/detail/component_vector.h"

using namespace phenyl::core;

UntypedComponentVector::UntypedComponentVector (std::size_t typeIndex, std::size_t dataSize, std::size_t startCapacity) : m_type{typeIndex}, m_compSize{dataSize}, m_size{0}, m_capacity{startCapacity} {
    m_memory = std::make_unique<std::byte[]>(dataSize * startCapacity);
}

UntypedComponentVector::UntypedComponentVector (UntypedComponentVector&& other) noexcept : m_type{other.m_type}, m_memory{std::move(other.m_memory)}, m_compSize{other.m_compSize}, m_size{other.m_size}, m_capacity{other.m_capacity} {
    other.m_compSize = 0;
    other.m_size = 0;
    other.m_capacity = 0;
}

UntypedComponentVector& UntypedComponentVector::operator= (UntypedComponentVector&& other) noexcept {
    PHENYL_DASSERT(m_type == other.m_type);
    if (m_memory) {
        deleteAllComps(m_memory.get(), m_memory.get() + m_compSize * m_capacity);
    }

    m_memory = std::move(other.m_memory);
    m_compSize = other.m_compSize;
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    return *this;
}

std::byte* UntypedComponentVector::insertUntyped () {
    guaranteeLength(m_size + 1);
    PHENYL_DASSERT(m_capacity >= m_size + 1);

    return m_memory.get() + (m_size++) * m_compSize;
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
    m_size--;
}

void UntypedComponentVector::clear() {
    deleteAllComps(m_memory.get(), m_memory.get() + m_size * m_compSize);
    m_size = 0;
}

void UntypedComponentVector::guaranteeLength (std::size_t newLen) {
    if (newLen <= m_capacity) {
        return;
    }

    std::size_t newCapacity = m_capacity * RESIZE_FACTOR;
    std::unique_ptr<std::byte[]> newMemory = std::make_unique<std::byte[]>(newCapacity * m_compSize);
    moveAllComps(m_memory.get(), m_memory.get() + m_capacity * m_compSize, newMemory.get());

    m_capacity = newCapacity;
    m_memory = std::move(newMemory);
}
