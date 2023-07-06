#pragma once

#include <memory>

#include "logging/logging.h"
#include "iterable.h"
#include <string>
#include <sstream>
namespace util {
    namespace detail {
        template <typename T>
        struct FlVectorItem {
        private:
            static constexpr std::size_t PRESENT_INDEX = 0;
        public:
            static constexpr std::size_t FREE_LIST_END = -1;
            alignas(T) unsigned char data[sizeof(T)] = {};
            std::size_t next = FREE_LIST_END;

            bool isPresent () {
                return !next;
            }

            T& getUnsafe () {
                return *reinterpret_cast<T*>(&data);
            }

            const T& getUnsafe () const {
                return *reinterpret_cast<T*>(&data);
            }

            template <typename ...Args>
            std::size_t init (Args&&... args) {
                if (isPresent()) {
                    logging::log(LEVEL_ERROR, "Attempting to init already present FLVectorItem!");
                }
                new (&getUnsafe()) T(std::forward<Args>(args)...);
                std::size_t oldNext = next;
                next = PRESENT_INDEX;

                return oldNext;
            }

            void destroy (std::size_t flHead) {
                if (isPresent()) {
                    getUnsafe().~T();
                }
                next = flHead;
            }

            FlVectorItem() : next{FREE_LIST_END} {}

            FlVectorItem (const FlVectorItem<T>& other) : next{other.next} {
                if (other.isPresent()) {
                    init(other.getUnsafe());
                }
            }

            FlVectorItem (FlVectorItem<T>&& other) noexcept : next{other.next} {
                if (other.isPresent()) {
                    init(std::move(other.getUnsafe()));
                    other.destroy(FREE_LIST_END);
                } else {
                    other.next = FREE_LIST_END;
                }
            }

            FlVectorItem<T>& operator= (const FlVectorItem<T>& other) {
                if (this == &other) {
                    return *this;
                }
                if (isPresent()) {
                    getUnsafe().~T();
                }

                next = other.next;
                if (other.isPresent()) {
                    init(other.getUnsafe());
                }

                return *this;
            }

            FlVectorItem<T>& operator= (FlVectorItem<T>&& other) noexcept {
                if (isPresent()) {
                    getUnsafe().~T();
                }

                next = other.next;
                if (other.isPresent()) {
                    init(other.getUnsafe());
                    other.destroy(FREE_LIST_END);
                }

                return *this;
            }


            ~FlVectorItem() {
                if (isPresent()) {
                    getUnsafe().~T();
                }
            }
        };

        template <class T>
        class FLVectorIterator {
        private:
            FlVectorItem<T>* items;
            std::size_t pos;
            std::size_t size;
            void increment () {
                pos++;
                while (pos < size && !items[pos].isPresent()) {
                    pos++;
                }
            }

            void findFirst () {
                while (pos < size && !items[pos].isPresent()) {
                    pos++;
                }
            }

            void decrement () {
                pos--;
                while (pos >= 0 && !items[pos].isPresent()) {
                    pos--;
                }
            }

        public:
            using iterator = FLVectorIterator<T>;
            using value_type = T;
            using reference = T&;
            //using const_reference = const T&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            FLVectorIterator() : items{nullptr}, size{0}, pos{0} {}
            FLVectorIterator (FlVectorItem<T>* items, size_t size, size_t pos = 0) : items{items}, size{size}, pos{pos} {
                findFirst();
            }

            reference operator* () const {
                return items[pos].getUnsafe();
            }

            iterator& operator++ () {
                increment();

                return *this;
            }

            iterator operator++ (int) {
                iterator old = *this;
                increment();

                return old;
            }

            bool operator== (const iterator& other) const {
                return (items == nullptr && other.items == nullptr) || (items == other.items && pos == other.pos);
            }

            iterator& operator-- () {
                decrement();
                return *this;
            }

            iterator operator-- (int) {
                iterator old = *this;
                decrement();

                return old;
            }
        };

        template <class T>
        class FLVectorPairIterator {
        private:
            FlVectorItem<T>* items;
            std::size_t pos;
            std::size_t size;
            void increment () {
                pos++;
                while (pos < size && !items[pos].isPresent()) {
                    pos++;
                }
            }

            void findFirst () {
                while (pos < size && !items[pos].isPresent()) {
                    pos++;
                }
            }

            void decrement () {
                pos--;
                while (pos >= 0 && !items[pos].isPresent()) {
                    pos--;
                }
            }

        public:
            using iterator = FLVectorPairIterator<T>;
            using value_type = std::pair<std::size_t, T&>;
            using reference = void;
            //using const_reference = const T&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            FLVectorPairIterator () : items{nullptr}, size{0}, pos{0} {}
            FLVectorPairIterator (FlVectorItem<T>* items, size_t size, size_t pos = 0) : items{items}, size{size}, pos{pos} {
                findFirst();
            }

            value_type operator* () const {
                return {pos, items[pos].getUnsafe()};
            }

            iterator& operator++ () {
                increment();

                return *this;
            }

            iterator operator++ (int) {
                iterator old = *this;
                increment();

                return old;
            }

            bool operator== (const iterator& other) const {
                return (items == nullptr && other.items == nullptr) || (items == other.items && pos == other.pos);
            }

            iterator& operator-- () {
                decrement();

                return *this;
            }

            iterator operator-- (int) {
                iterator old = *this;
                decrement();

                return old;
            }
        };
    }

    template <class T>
    class FLVector {
    private:
        using VectorItem = detail::FlVectorItem<T>;
        static constexpr std::size_t DEFAULT_START = 16;
        static constexpr std::size_t RESIZE_RATIO = 2;
        std::unique_ptr<VectorItem[]> data;
        std::size_t flHead = VectorItem::FREE_LIST_END;

        std::size_t presentNum = 0;
        std::size_t listSize = 0;
        std::size_t vecCapacity  = 0;

        std::size_t getNextIndex () {
            if (flHead != VectorItem::FREE_LIST_END) {
                auto index = flHead;
                flHead = data[flHead].next;

                return index;
            } else if (listSize == vecCapacity) {
                reserve(listSize * RESIZE_RATIO);
            }

            return listSize++;
        }

    public:
        using iterator = detail::FLVectorIterator<T>;
        using const_iterator = detail::FLVectorIterator<const T>;
        using pair_iterator = detail::FLVectorPairIterator<T>;
        FLVector () : FLVector(DEFAULT_START) {}

        explicit FLVector (std::size_t capacity) : vecCapacity{capacity} {
            data = std::make_unique<VectorItem[]>(capacity);
        }

        FLVector (const FLVector<T>& other) : presentNum{other.presentNum}, listSize{other.listSize}, vecCapacity{other.vecCapacity}, flHead{other.flHead} {
            data = std::make_unique<VectorItem[]>(vecCapacity);

            for (std::size_t i = 0; i < other.listSize; i++) {
                data[i] = other.data[i];
            }
        }

        FLVector (FLVector<T>&& other) noexcept : presentNum{other.presentNum}, listSize{other.listSize}, vecCapacity{other.vecCapacity}, flHead{other.flHead} {
            data = std::move(other.data);
            other.data = nullptr;
            other.presentNum = 0;
            other.listSize = 0;
            other.vecCapacity = 0;
            other.flHead = VectorItem::FREE_LIST_END;
        }

        FLVector<T>& operator= (const FLVector<T>& other) {
            if (this == &other) {
                return *this;
            }

            if (vecCapacity < other.listSize) {
                data = std::make_unique<VectorItem[]>(other.vecCapacity);
                vecCapacity = other.vecCapacity;
                listSize = 0;
            }

            for (std::size_t i = 0; i < other.listSize; i++) {
                data[i] = other.data[i];
            }

            for (std::size_t i = other.listSize; i < listSize; i++) {
                data[i].destroy(VectorItem::FREE_LIST_END);
            }

            listSize = other.listSize;
            flHead = other.flHead;
            presentNum = other.presentNum;

            return *this;
        }

        FLVector<T>& operator= (FLVector<T>&& other) noexcept {
            data = std::move(other.data);
            flHead = other.flHead;
            presentNum = other.presentNum;
            listSize = other.listSize;
            vecCapacity = other.vecCapacity;

            other.flHead = 0;
            other.presentNum = 0;
            other.listSize = 0;
            other.vecCapacity = 0;

            return *this;
        }


        T& operator[] (std::size_t index) {
            data[index].getUnsafe();
        }

        const T& operator[] (std::size_t index) const {
            data[index].getUnsafe();
        }

        T& at (std::size_t index) {
            if (index > listSize || !data[index].isPresent()) {
                throw std::out_of_range("Attempted to access invalid element!");
            }

            return data[index].getUnsafe();
        }

        const T& at (std::size_t index) const {
            if (index > listSize || !data[index].isPresent()) {
                throw std::out_of_range("Attempted to access invalid element!");
            }

            return data[index].getUnsafe();
        }

        [[nodiscard]] bool empty () const {
            return !presentNum;
        }

        [[nodiscard]] std::size_t size () const {
            return presentNum;
        }

        [[nodiscard]] std::size_t capacity () const {
            return vecCapacity;
        }

        void reserve (std::size_t newCapacity) {
            if (vecCapacity >= newCapacity) {
                return;
            }

            auto newData = std::make_unique<VectorItem[]>(newCapacity);

            for (std::size_t i = 0; i < listSize; i++) {
                newData[i] = std::move(data[i]);
            }

            data = std::move(newData);
            vecCapacity = newCapacity;
        }

        void clear () {
            for (std::size_t i = 0; i < listSize; i++) {
                data[i].destroy(VectorItem::FREE_LIST_END);
            }

            listSize = 0;
            presentNum = 0;
            flHead = VectorItem::FREE_LIST_END;
        }

        std::size_t push (const T& val) {
            auto index = getNextIndex();

            data[index].init(val);
            presentNum++;
            return index;
        }

        std::size_t push (T&& val) {
            auto index = getNextIndex();

            data[index].init(std::forward<T>(val));
            presentNum++;
            return index;
        }

        template <typename ...Args>
        std::size_t emplace (Args&&... args) {
            auto index = getNextIndex();

            data[index].init(std::forward<Args>(args)...);
            presentNum++;
            return index;
        }

        void remove (std::size_t index) {
            if (!data[index].isPresent()) {
                logging::log(LEVEL_ERROR, "Attempted to remove already removed element of FLVector!");
                return;
            }

            data[index].destroy(flHead);
            flHead = index;
        }

        bool present (std::size_t index) const {
            return listSize > index && data[index].isPresent();
        }

        iterator begin () noexcept {
            return iterator{data.get(), listSize};
        }

        iterator end () noexcept {
            return iterator{data.get(), listSize, listSize};
        }

        const_iterator begin () const noexcept {
            return const_iterator{data.get(), listSize};
        }

        const_iterator end () const noexcept {
            return iterator{data.get(), listSize, listSize};
        }

        const_iterator cbegin () const noexcept {
            return const_iterator{data.get(), listSize};
        }

        const_iterator cend () const noexcept {
            return iterator{data.get(), listSize, listSize};
        }

        util::Iterable<pair_iterator> iterate () {
            return {pair_iterator{data.get(), listSize}, pair_iterator{data.get(), listSize, listSize}};
        }

        template <bool b = true>
        std::string toString () {
            std::stringstream sstream;
            sstream << "[";
            for (std::size_t i = 0; i < listSize; i++) {
                if (i != 0) {
                    sstream << ", ";
                }
                if (data[i].isPresent()) {
                    sstream << data[i].getUnsafe();
                } else {
                    sstream << "NULL";
                }
            }
            sstream << "]";

            return sstream.str();
        }
    };
}