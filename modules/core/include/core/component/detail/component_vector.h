#pragma once

#include "logging/logging.h"
#include "util/type_index.h"

#include <memory>

namespace phenyl::core {
class UntypedComponentVector {
public:
    UntypedComponentVector (meta::TypeIndex typeIndex, std::size_t dataSize, std::size_t startCapacity);
    virtual ~UntypedComponentVector () = default;

    UntypedComponentVector (UntypedComponentVector&& other) noexcept;
    UntypedComponentVector& operator= (UntypedComponentVector&& other) noexcept;

    std::byte* getUntyped (std::size_t pos) {
        PHENYL_DASSERT(pos < size());
        return m_memory.get() + (pos * m_compSize);
    }

    [[nodiscard]] const std::byte* getUntyped (std::size_t pos) const {
        PHENYL_DASSERT(pos < size());
        return m_memory.get() + (pos * m_compSize);
    }

    std::byte* insertUntyped ();
    void moveFrom (UntypedComponentVector& other, std::size_t pos);
    void remove (std::size_t pos);
    void clear ();

    [[nodiscard]] meta::TypeIndex type () const noexcept {
        return m_type;
    }

    [[nodiscard]] std::size_t size () const noexcept {
        return m_size;
    }

    [[nodiscard]] std::size_t capacity () const noexcept {
        return m_capacity;
    }

    [[nodiscard]] std::size_t stride () const noexcept {
        return m_compSize;
    }

    virtual std::unique_ptr<UntypedComponentVector> makeNew (std::size_t startCapacity = 16) const = 0;

protected:
    virtual void moveComp (std::byte* from, std::byte* to) = 0;
    virtual void moveConstructComp (std::byte* from, std::byte* to) = 0;
    virtual void deleteComp (std::byte* comp) = 0;
    virtual void moveAllComps (std::byte* start, std::byte* end, std::byte* newStart) = 0;
    virtual void deleteAllComps (std::byte* start, std::byte* end) = 0;

    std::byte* beginUntyped () {
        return m_memory.get();
    }

    [[nodiscard]] const std::byte* beginUntyped () const {
        return m_memory.get();
    }

    std::byte* endUntyped () {
        return m_memory.get() + (m_size * m_compSize);
    }

    const std::byte* endUntyped () const {
        return m_memory.get() + (m_size * m_compSize);
    }

private:
    static constexpr std::size_t RESIZE_FACTOR = 2;

    meta::TypeIndex m_type;

    std::unique_ptr<std::byte[]> m_memory;
    std::size_t m_compSize;
    std::size_t m_size;
    std::size_t m_capacity;

    void guaranteeLength (std::size_t newLen);
};

template <typename T>
class ComponentVector : public UntypedComponentVector {
public:
    using iterator = T*;
    using const_iterator = const T*;

    explicit ComponentVector (std::size_t startCapacity = 16) :
        UntypedComponentVector{meta::TypeIndex::Get<T>(), sizeof(T), startCapacity} {}

    ~ComponentVector () override {
        auto* start = reinterpret_cast<T*>(beginUntyped());
        auto* end = reinterpret_cast<T*>(endUntyped());

        for (auto* i = start; i < end; i++) {
            i->~T();
        }
    }

    template <typename... Args>
    T* emplace (Args&&... args) requires std::constructible_from<T, Args&&...>
    {
        T* ptr = reinterpret_cast<T*>(insertUntyped());

        new (ptr) T(std::forward<Args>(args)...);
        return ptr;
    }

    [[nodiscard]] std::unique_ptr<UntypedComponentVector> makeNew (std::size_t startCapacity) const override {
        return std::make_unique<ComponentVector<T>>(startCapacity);
    }

    T& operator[] (std::size_t pos) {
        return *reinterpret_cast<T*>(getUntyped(pos));
    }

    const T& operator[] (std::size_t pos) const {
        return *reinterpret_cast<T*>(getUntyped(pos));
    }

    iterator begin () {
        return iterator{reinterpret_cast<T*>(beginUntyped())};
    }

    iterator end () {
        return iterator{reinterpret_cast<T*>(endUntyped())};
    }

    const_iterator begin () const {
        return cbegin();
    }

    const_iterator cbegin () const {
        return const_iterator{reinterpret_cast<T*>(beginUntyped())};
    }

    const_iterator end () const {
        return cend();
    }

    const_iterator cend () const {
        return const_iterator{reinterpret_cast<T*>(endUntyped())};
    }

protected:
    void moveComp (std::byte* from, std::byte* to) override {
        auto* fromTyped = reinterpret_cast<T*>(from);
        auto* toTyped = reinterpret_cast<T*>(to);

        *toTyped = std::move(*fromTyped);
    }

    void moveConstructComp (std::byte* from, std::byte* to) override {
        auto* fromTyped = reinterpret_cast<T*>(from);
        auto* toTyped = reinterpret_cast<T*>(to);

        new (toTyped) T(std::move(*fromTyped));
    }

    void deleteComp (std::byte* comp) override {
        auto* compTyped = reinterpret_cast<T*>(comp);

        compTyped->~T();
    }

    void moveAllComps (std::byte* start, std::byte* end, std::byte* newStart) override {
        auto* startTyped = reinterpret_cast<T*>(start);
        auto* endTyped = reinterpret_cast<T*>(end);
        auto* newStartTyped = reinterpret_cast<T*>(newStart);

        for (auto* i = startTyped; i < endTyped; i++) {
            *(newStartTyped++) = std::move(*i);
            i->~T();
        }
    }

    void deleteAllComps (std::byte* start, std::byte* end) override {
        auto* startTyped = reinterpret_cast<T*>(start);
        auto* endTyped = reinterpret_cast<T*>(end);

        for (auto* i = startTyped; i < endTyped; i++) {
            i->~T();
        }
    }
};
} // namespace phenyl::core
