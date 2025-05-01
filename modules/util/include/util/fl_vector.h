#pragma once

#include <memory>

#include "logging/logging.h"
#include "iterable.h"
#include <string>
#include <sstream>

#include "detail/loggers.h"

namespace phenyl::util {
    namespace detail {
        template <typename T>
        struct FlVectorItem {
        public:
            static constexpr std::size_t FREE_LIST_END = -1;
            alignas(T) unsigned char data[sizeof(T)] = {};
            std::size_t next = FREE_LIST_END;

            bool isPresent () const {
                return !next;
            }

            T& getUnsafe () {
                PHENYL_DASSERT(isPresent());
                return *reinterpret_cast<T*>(&data);
            }

            const T& getUnsafe () const {
                PHENYL_DASSERT(isPresent());
                return *reinterpret_cast<T*>(&data);
            }

            template <typename ...Args>
            std::size_t init (Args&&... args) {
                /*if (isPresent()) {
                    logging::log(LEVEL_ERROR, "Attempting to init already present FLVectorItem!");
                }*/
                PHENYL_ASSERT_MSG(!isPresent(), "Attempting to init already present FLVectorItem!");

                new (&data) T(std::forward<Args>(args)...);
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
                    next = FREE_LIST_END;
                }

                if (other.isPresent()) {
                    init(other.getUnsafe());
                }

                next = other.next;

                return *this;
            }

            FlVectorItem<T>& operator= (FlVectorItem<T>&& other) noexcept {
                if (isPresent()) {
                    getUnsafe().~T();
                    next = FREE_LIST_END;
                }

                if (other.isPresent()) {
                    init(std::move(other.getUnsafe()));
                    next = other.next;

                    other.destroy(FREE_LIST_END);
                }


                return *this;
            }


            ~FlVectorItem() {
                if (isPresent()) {
                    getUnsafe().~T();
                }
            }

        private:
            static constexpr std::size_t PRESENT_INDEX = 0;
        };

        template <class T>
        class FLVectorIterator {
        public:
            using iterator = FLVectorIterator<T>;
            using value_type = T;
            using reference = T&;
            //using const_reference = const T&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            FLVectorIterator() : m_items{nullptr}, m_size{0}, m_pos{0} {}
            FLVectorIterator (FlVectorItem<T>* items, size_t size, size_t pos = 0) : m_items{items}, m_size{size}, m_pos{pos} {
                findFirst();
            }

            reference operator* () const {
                return m_items[m_pos].getUnsafe();
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
                return (m_items == nullptr && other.m_items == nullptr) || (m_items == other.m_items && m_pos == other.m_pos);
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

        private:
            FlVectorItem<T>* m_items;
            std::size_t m_pos;
            std::size_t m_size;
            void increment () {
                m_pos++;
                while (m_pos < m_size && !m_items[m_pos].isPresent()) {
                    m_pos++;
                }
            }

            void findFirst () {
                while (m_pos < m_size && !m_items[m_pos].isPresent()) {
                    m_pos++;
                }
            }

            void decrement () {
                m_pos--;
                while (m_pos >= 0 && !m_items[m_pos].isPresent()) {
                    m_pos--;
                }
            }
        };

        template <class T>
        class FLVectorPairIterator {
        public:
            using iterator = FLVectorPairIterator<T>;
            using value_type = std::pair<std::size_t, T&>;
            using reference = void;
            //using const_reference = const T&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            FLVectorPairIterator () : m_items{nullptr}, m_size{0}, m_pos{0} {}
            FLVectorPairIterator (FlVectorItem<T>* items, size_t size, size_t pos = 0) : m_items{items}, m_size{size}, m_pos{pos} {
                findFirst();
            }

            value_type operator* () const {
                return {m_pos, m_items[m_pos].getUnsafe()};
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
                return (m_items == nullptr && other.m_items == nullptr) || (m_items == other.m_items && m_pos == other.m_pos);
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

        private:
            FlVectorItem<T>* m_items;
            std::size_t m_pos;
            std::size_t m_size;
            void increment () {
                m_pos++;
                while (m_pos < m_size && !m_items[m_pos].isPresent()) {
                    m_pos++;
                }
            }

            void findFirst () {
                while (m_pos < m_size && !m_items[m_pos].isPresent()) {
                    m_pos++;
                }
            }

            void decrement () {
                m_pos--;
                while (m_pos >= 0 && !m_items[m_pos].isPresent()) {
                    m_pos--;
                }
            }
        };
    }

    template <class T>
    class FLVector {
    public:
        using iterator = detail::FLVectorIterator<T>;
        using const_iterator = detail::FLVectorIterator<const T>;
        using pair_iterator = detail::FLVectorPairIterator<T>;
        FLVector () : FLVector(DEFAULT_START) {}

        explicit FLVector (std::size_t capacity) : m_capacity{capacity} {
            m_data = std::make_unique<VectorItem[]>(capacity);
        }

        FLVector (const FLVector<T>& other) : m_presentNum{other.m_presentNum}, m_listSize{other.m_listSize}, m_capacity{other.m_capacity}, m_flHead{other.m_flHead} {
            m_data = std::make_unique<VectorItem[]>(m_capacity);

            for (std::size_t i = 0; i < other.m_listSize; i++) {
                m_data[i] = other.m_data[i];
            }
        }

        FLVector (FLVector<T>&& other) noexcept : m_presentNum{other.m_presentNum}, m_listSize{other.m_listSize}, m_capacity{other.m_capacity}, m_flHead{other.m_flHead} {
            m_data = std::move(other.m_data);
            other.m_data = nullptr;
            other.m_presentNum = 0;
            other.m_listSize = 0;
            other.m_capacity = 0;
            other.m_flHead = VectorItem::FREE_LIST_END;
        }

        FLVector<T>& operator= (const FLVector<T>& other) {
            if (this == &other) {
                return *this;
            }

            if (m_capacity < other.m_listSize) {
                m_data = std::make_unique<VectorItem[]>(other.m_capacity);
                m_capacity = other.m_capacity;
                m_listSize = 0;
            }

            for (std::size_t i = 0; i < other.m_listSize; i++) {
                m_data[i] = other.m_data[i];
            }

            for (std::size_t i = other.m_listSize; i < m_listSize; i++) {
                m_data[i].destroy(VectorItem::FREE_LIST_END);
            }

            m_listSize = other.m_listSize;
            m_flHead = other.m_flHead;
            m_presentNum = other.m_presentNum;

            return *this;
        }

        FLVector<T>& operator= (FLVector<T>&& other) noexcept {
            m_data = std::move(other.m_data);
            m_flHead = other.m_flHead;
            m_presentNum = other.m_presentNum;
            m_listSize = other.m_listSize;
            m_capacity = other.m_capacity;

            other.m_flHead = 0;
            other.m_presentNum = 0;
            other.m_listSize = 0;
            other.m_capacity = 0;

            return *this;
        }


        T& operator[] (std::size_t index) {
            return m_data[index].getUnsafe();
        }

        const T& operator[] (std::size_t index) const {
            return m_data[index].getUnsafe();
        }

        T& at (std::size_t index) {
            if (index > m_listSize || !m_data[index].isPresent()) {
                throw std::out_of_range("Attempted to access invalid element!");
            }

            return m_data[index].getUnsafe();
        }

        const T& at (std::size_t index) const {
            if (index > m_listSize || !m_data[index].isPresent()) {
                throw std::out_of_range("Attempted to access invalid element!");
            }

            return m_data[index].getUnsafe();
        }

        [[nodiscard]] bool empty () const {
            return !m_presentNum;
        }

        [[nodiscard]] std::size_t size () const {
            return m_presentNum;
        }

        [[nodiscard]] std::size_t capacity () const {
            return m_capacity;
        }

        void reserve (std::size_t newCapacity) {
            if (m_capacity >= newCapacity) {
                return;
            }

            auto newData = std::make_unique<VectorItem[]>(newCapacity);

            for (std::size_t i = 0; i < m_listSize; i++) {
                newData[i] = std::move(m_data[i]);
            }

            m_data = std::move(newData);
            m_capacity = newCapacity;
        }

        void clear () {
            for (std::size_t i = 0; i < m_listSize; i++) {
                m_data[i].destroy(VectorItem::FREE_LIST_END);
            }

            m_listSize = 0;
            m_presentNum = 0;
            m_flHead = VectorItem::FREE_LIST_END;
        }

        std::size_t push (const T& val) {
            auto index = getNextIndex();

            m_data[index].init(val);
            m_presentNum++;
            return index;
        }

        std::size_t push (T&& val) {
            auto index = getNextIndex();

            m_data[index].init(std::move(val));
            m_presentNum++;
            return index;
        }

        template <typename ...Args>
        std::size_t emplace (Args&&... args) {
            auto index = getNextIndex();

            m_data[index].init(std::forward<Args>(args)...);
            m_presentNum++;
            return index;
        }

        void remove (std::size_t index) {
            PHENYL_ASSERT_MSG(index < m_listSize, "Attempted to remove invalid element {} of FLVector!", index);
            PHENYL_ASSERT_MSG(m_data[index].isPresent(), "Attempted to remove already removed element {} of FLVector!", index);

            m_data[index].destroy(m_flHead);
            m_flHead = index + 1;
            m_presentNum--;
        }

        bool present (std::size_t index) const {
            return m_listSize > index && m_data[index].isPresent();
        }

        iterator begin () noexcept {
            return iterator{m_data.get(), m_listSize};
        }

        iterator end () noexcept {
            return iterator{m_data.get(), m_listSize, m_listSize};
        }

        const_iterator begin () const noexcept {
            return const_iterator{m_data.get(), m_listSize};
        }

        const_iterator end () const noexcept {
            return iterator{m_data.get(), m_listSize, m_listSize};
        }

        const_iterator cbegin () const noexcept {
            return const_iterator{m_data.get(), m_listSize};
        }

        const_iterator cend () const noexcept {
            return iterator{m_data.get(), m_listSize, m_listSize};
        }

        util::Iterable<pair_iterator> iterate () {
            return {pair_iterator{m_data.get(), m_listSize}, pair_iterator{m_data.get(), m_listSize, m_listSize}};
        }

        template <bool b = true>
        std::string toString () {
            std::stringstream sstream;
            sstream << "[";
            for (std::size_t i = 0; i < m_listSize; i++) {
                if (i != 0) {
                    sstream << ", ";
                }
                if (m_data[i].isPresent()) {
                    sstream << m_data[i].getUnsafe();
                } else {
                    sstream << "NULL";
                }
            }
            sstream << "]";

            return sstream.str();
        }

    private:
        using VectorItem = detail::FlVectorItem<T>;
        static constexpr std::size_t DEFAULT_START = 16;
        static constexpr std::size_t RESIZE_RATIO = 2;
        std::unique_ptr<VectorItem[]> m_data;
        std::size_t m_flHead = VectorItem::FREE_LIST_END;

        std::size_t m_presentNum = 0;
        std::size_t m_listSize = 0;
        std::size_t m_capacity  = 0;

        std::size_t getNextIndex () {
            if (m_flHead != VectorItem::FREE_LIST_END) {
                auto index = m_flHead - 1;
                m_flHead = m_data[index].next;

                return index;
            } else if (m_listSize == m_capacity) {
                reserve(m_listSize * RESIZE_RATIO);
            }

            return m_listSize++;
        }
    };
}
