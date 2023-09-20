#pragma once

#include <cstring>
#include <memory>
#include <utility>

namespace phenyl::util {
    namespace detail {
        template <typename K>
        struct SetElement {
            alignas(alignof(K)) unsigned char data[sizeof(K)];
            std::size_t hash = 0;
            char state = 0; // 0 == non-existent, 1 = exists, -1 = deleted;

            SetElement() = default;

            SetElement (const SetElement<K>& other) {
                state = other.state;
                hash = other.hash;

                if (other.state == 1) {
                    new (getDataPtr()) K (*other.getDataPtr());
                }
            }
            SetElement (SetElement<K>&& other)  noexcept {
                if (state == 1) {
                    destructData();
                }
                if (other.state == 1) {
                    new (getDataPtr()) K (std::move(*other.getDataPtr()));
                    other.getDataPtr()->~K();
                    hash = other.hash;
                    state = 1;
                    other.state = -1;
                }
            }

            SetElement<K>& operator= (const SetElement<K>& other) {
                if (&other == this) {
                    return *this;
                }

                if (state == 1) {
                    destructData();
                }

                state = other.state;
                hash = other.hash;

                if (other.state == 1) {
                    new (getDataPtr()) K (*other.getDataPtr());
                }

                return *this;
            };
            SetElement<K>& operator= (SetElement&& other)  noexcept {
                if (other.state == 1) {
                    //*getDataPtr() = std::move(*other.getDataPtr());
                    new (getDataPtr()) K (std::move(*other.getDataPtr()));
                    other.getDataPtr()->~K();
                    hash = other.hash;
                    state = 1;
                    other.state = 0;
                }

                return *this;
            }

            inline bool exists () {
                return state == 1;
            }

            inline bool free () {
                return state != 1;
            }

            inline bool occupied () {
                return state != 0;
            }

            inline bool deleted () {
                return state == -1;
            }

            template <typename ...Args>
            void constructData (std::size_t hashVal, Args&&... args) {
                new (getDataPtr()) K (std::forward<Args>(args)...);
                state = 1;
                hash = hashVal;
            }

            void destructData () {
                getDataPtr()->~K();
                state = -1;
            }

            void clear () {
                if (state == 1) {
                    destructData();
                }
                state = 0;
            }


            K* getDataPtr () const {
                return (K*)data;
            }

            ~SetElement() {
                if (state == 1) {
                    destructData();
                }
            }
        };

        template <class K>
        struct SetIterator {
        private:
            detail::SetElement<K>* data;
            std::size_t pos;
            std::size_t maxSize;
        public:
            SetIterator() : data{nullptr}, pos{0}, maxSize{0} {}
            SetIterator (detail::SetElement<K>* _data, std::size_t _maxSize, std::size_t _pos = 0) : data{_data}, pos{_pos}, maxSize{_maxSize} {}

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = K;
            using element_type = K;
            using reference = const K&;

            reference operator* () const {
                return *data[pos].getDataPtr();
            }

            reference operator-> () const {
                return *data[pos].getDataPtr();
            }

            SetIterator<K>& operator++ () {
                std::size_t lastPos = pos;
                while (pos < maxSize && (pos == lastPos || !data[pos].exists())) {
                    pos++;
                }
                return *this;
            }

            SetIterator<K> operator++ (int) {
                SetIterator<K> other = *this;
                return ++other;
            }

            SetIterator<K>& operator-- () {
                std::size_t lastPos = pos;
                while (pos >= 0 && (pos == lastPos || !data[pos].exists())) {
                    pos--;
                }
            }

            SetIterator<K> operator-- (int) {
                SetIterator<K> other = *this;
                return --other;
            }

            bool operator== (const SetIterator<K>& other) const {
                return data == other.data && pos == other.pos;
            }
        };
    }


    template <class K, typename HashFunc = std::hash<K>>
    class BasicSet {
    private:
        std::unique_ptr<detail::SetElement<K>[]> data;
        float loadFactor = 0.75f;
        float resizeRatio = 2.0f;
        std::size_t maxSize = 16;
        std::size_t currentSize = 0;
        detail::SetElement<K>* findElement (std::size_t hash) const {
            std::size_t pos = hash % maxSize;

            detail::SetElement<K>* firstDeleted = nullptr;

            while (data[pos].occupied()) {
                if (data[pos].hash == hash) {
                    return &data[pos];
                } else if (!firstDeleted && data[pos].deleted()) {
                    firstDeleted = &data[pos];
                }
                pos = (pos + 1) % maxSize;
            }

            return firstDeleted == nullptr ? &data[pos] : firstDeleted;
        }

        void resizeIfNecessary () {
            if ((std::size_t)(maxSize * loadFactor) > (currentSize + 1)) {
                return;
            }

            std::size_t newMaxSize = (int)(maxSize * resizeRatio);
            auto newData = std::make_unique<detail::SetElement<K>[]>(newMaxSize);

            for (std::size_t i = 0; i < maxSize; i++) {
                if (!data[i].exists()) {
                    continue;
                }

                std::size_t newIndex = data[i].hash % newMaxSize;
                while (!newData[newIndex].free()) {
                    newIndex = (newIndex + 1) % newMaxSize;
                }

                newData[newIndex] = std::move(data[i]);
            }

            data = std::move(newData);
            maxSize = newMaxSize;
        }

    public:
        using iterator = detail::SetIterator<K>;
        using const_iterator = detail::SetIterator<const K>;
        BasicSet () {
            data = std::make_unique<detail::SetElement<K>[]>(maxSize);
        }

        BasicSet (const BasicSet<K, HashFunc>& other) {
            maxSize = other.maxSize;
            currentSize = other.currentSize;

            data = std::make_unique<detail::SetElement<K>[]>(maxSize);

            for (std::size_t i = 0; i < maxSize; i++) {
                if (other.data[i].exists()) {
                    data[i] = other.data[i];
                }
            }
        }

        BasicSet (BasicSet<K, HashFunc>&& other) noexcept {
            maxSize = other.maxSize;
            currentSize = other.currentSize;
            data = std::move(other.data);
        }

        BasicSet<K, HashFunc>& operator= (const BasicSet<K, HashFunc>& other) {
            if (&other == this) {
                return *this;
            }

            maxSize = other.maxSize;
            currentSize = other.currentSize;

            data = std::make_unique<detail::SetElement<K>[]>(maxSize);

            for (std::size_t i = 0; i < maxSize; i++) {
                if (other.data[i].exists()) {
                    data[i] = other.data[i];
                }
            }

            return *this;
        }

        BasicSet<K, HashFunc>& operator= (BasicSet<K, HashFunc>&& other) noexcept {
            maxSize = other.maxSize;
            currentSize = other.currentSize;
            data = std::move(other.data);

            return *this;
        }

        [[nodiscard]] std::size_t size () const {
            return currentSize;
        }

        [[nodiscard]] bool empty () const {
            return currentSize == 0;
        }

        void clear () {
            for (std::size_t i = 0; i < maxSize; i++) {
                data[i].clear();
            }

            currentSize = 0;
        }

        void insert (K&& k) {
            resizeIfNecessary();
            auto hashFunc = HashFunc();
            auto hash = hashFunc(k);
            auto ptr = findElement(hash);

            if (!ptr->exists()) {
                ptr->constructData(hash, std::forward<K>(k));
                currentSize++;
            }
        }

        void insert (const K& k) {
            auto hashFunc = HashFunc();
            auto hash = hashFunc(k);
            auto ptr = findElement(hash);

            if (!ptr->exists()) {
                resizeIfNecessary();
                ptr->constructData(hash, k);
                currentSize++;
            }
        }

        template <typename ...Args>
        void emplace (Args&&... args) {
            insert(K(std::forward<Args>(args)...));
        }

        void remove (const K& k) {
            auto hashFunc = HashFunc();
            auto hash = hashFunc(k);
            auto ptr = findElement(hash);

            if (ptr->exists()) {
                ptr->destructData();
                currentSize--;
            }
        }

        bool contains (const K& k) const {
            auto hashFunc = HashFunc();
            auto hash = hashFunc(k);
            auto ptr = findElement(hash);

            return ptr->exists();
        }

        bool operator[] (const K& k) const {
            return contains(k);
        }

        iterator begin () {
            return iterator(data.get(), maxSize, 0);
        }

        iterator end () {
            return iterator(data.get(), maxSize, maxSize);
        }

        const_iterator cbegin () const {
            return const_iterator(data.get(), maxSize, 0);
        }

        const_iterator cend () const {
            return const_iterator(data.get(), maxSize, maxSize);
        }

    };

    void setTest ();

    template <typename K>
    using Set = BasicSet<K, std::hash<K>>;

}