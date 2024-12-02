#pragma once

#include <memory>

#include "logging/logging.h"
#include "util/meta.h"

namespace phenyl::core {
    class UntypedComponentVector {
    private:
        static constexpr std::size_t RESIZE_FACTOR = 2;

        std::size_t typeIndex;

        std::unique_ptr<std::byte[]> memory;
        std::size_t compSize;
        std::size_t vecLength;
        std::size_t vecCapacity;

        void guaranteeLength (std::size_t newLen);

    protected:
        virtual void moveComp (std::byte* from, std::byte* to) = 0;
        virtual void moveConstructComp (std::byte* from, std::byte* to) = 0;
        virtual void deleteComp (std::byte* comp) = 0;
        virtual void moveAllComps (std::byte* start, std::byte* end, std::byte* newStart) = 0;
        virtual void deleteAllComps (std::byte* start, std::byte* end) = 0;

        std::byte* beginUntyped () {
            return memory.get();
        }

        [[nodiscard]] const std::byte* beginUntyped () const {
            return memory.get();
        }

        std::byte* endUntyped () {
            return memory.get() + (vecLength * compSize);
        }

        const std::byte* endUntyped () const {
            return memory.get() + (vecLength * compSize);
        }
    public:
        UntypedComponentVector (std::size_t typeIndex, std::size_t dataSize, std::size_t startCapacity);
        virtual ~UntypedComponentVector() = default;

        UntypedComponentVector (UntypedComponentVector&& other) noexcept;
        UntypedComponentVector& operator= (UntypedComponentVector&& other) noexcept;

        std::byte* getUntyped (std::size_t pos) {
            PHENYL_DASSERT(pos < size());
            return memory.get() + (pos * compSize);
        }

        [[nodiscard]] const std::byte* getUntyped (std::size_t pos) const {
            PHENYL_DASSERT(pos < size());
            return memory.get() + (pos * compSize);
        }

        std::byte* insertUntyped ();
        void moveFrom (UntypedComponentVector& other, std::size_t pos);
        void remove (std::size_t pos);
        void clear ();


        [[nodiscard]] std::size_t type () const noexcept {
            return typeIndex;
        }

        [[nodiscard]] std::size_t size () const noexcept {
            return vecLength;
        }

        [[nodiscard]] std::size_t capacity () const noexcept {
            return vecCapacity;
        }

        virtual std::unique_ptr<UntypedComponentVector> makeNew (std::size_t startCapacity = 16) const = 0;
    };

    template <typename T>
    class ComponentVector : public UntypedComponentVector {
    protected:
        void moveComp (std::byte* from, std::byte* to) override {
            auto* fromTyped = reinterpret_cast<T*>(from);
            auto* toTyped = reinterpret_cast<T*>(to);

            *toTyped = std::move(*fromTyped);
        }

        void moveConstructComp(std::byte* from, std::byte* to) override {
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

    public:
        using iterator = T*;
        using const_iterator = const T*;

        explicit ComponentVector (std::size_t startCapacity = 16) : UntypedComponentVector{meta::type_index<T>(), sizeof(T), startCapacity} {}
        ~ComponentVector() override {
            auto* start = reinterpret_cast<T*>(beginUntyped());
            auto* end = reinterpret_cast<T*>(endUntyped());

            for (auto* i = start; i < end; i++) {
                i->~T();
            }
        }

        template <typename ...Args>
        T* emplace (Args&&... args) requires std::constructible_from<T, Args&&...> {
            T* ptr = reinterpret_cast<T*>(insertUntyped());

            new (ptr) T(std::forward<Args>(args)...);
            return ptr;
        }

        [[nodiscard]] std::unique_ptr<UntypedComponentVector> makeNew (std::size_t startCapacity) const override {
            return std::make_unique<ComponentVector<T>>(startCapacity);
        };

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
            return cend ();
        }
        const_iterator cend () const {
            return const_iterator{reinterpret_cast<T*>(endUntyped())};
        }
    };
}