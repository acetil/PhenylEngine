#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <iterator>
//#include <sstream>
#include <cstring>
#include <cstdint>

#include "util/meta.h"

namespace util {
    namespace detail {
        template <typename K, typename V>
        struct MapPair {
            alignas(alignof(K)) unsigned char keyData[sizeof(K)];
            alignas(alignof(V)) unsigned char valueData[sizeof(V)];
            std::size_t hash = 0;
            char state = 0;

            MapPair() = default;

            MapPair<K, V>& operator= (const MapPair<K, V>& other) {
                if (this == &other) {
                    return *this;
                }
                if (state == 1) {
                    destructData();
                }

                hash = other.hash;
                state = other.state;

                if (other.state == 1) {
                    new(getKeyPtr()) K(*other.getKeyPtr());
                    new(getValPtr()) V(*other.getValPtr());
                }

                return *this;
            }

            MapPair<K, V>& operator= (MapPair<K, V>&& other)  noexcept {
                if (state == 1) {
                    destructData();
                }

                hash = other.hash;
                state = other.state;

                if (other.state == 1) {
                    new (getKeyPtr()) K (std::move(*other.getKeyPtr()));
                    new (getValPtr()) V (std::move(*other.getValPtr()));

                    other.getKeyPtr()->~K();
                    other.getValPtr()->~V();

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
            void constructKey (std::size_t _hash, Args&&... args) {
                new (getKeyPtr()) K(std::forward<Args>(args)...);

                constructValue();
                hash = _hash;
                state = 1;
            }

            template <typename ...Args>
            void emplaceKValue (std::size_t _hash, const K& key, Args&&... args) {
                new (getKeyPtr()) K(key);

                constructValue(std::forward<Args>(args)...);
                hash = _hash;
                state = 1;
            }

            template <typename ...Args>
            void constructValue (Args&&... args) {
                if (state == 1) {
                    getValPtr()->~V();
                }
                new (getValPtr()) V(std::forward<Args>(args)...);
            }

            void destructData () {
                getKeyPtr()->~K();
                getValPtr()->~V();
                state = -1;
            }

            void clear () {
                if (state == 1) {
                    destructData();
                }
                state = 0;
            }

            K* getKeyPtr () const {
                return (K*)keyData;
            }

            V* getValPtr () const {
                return (V*)valueData;
            }

            ~MapPair() {
                if (state == 1) {
                    destructData();
                }
            }
        };

        template <typename K, typename V>
        class BasicMapIterator {
        private:
            detail::MapPair<K, V>* data;
            std::size_t pos;
            std::size_t maxSize;

        public:
            BasicMapIterator () : data{nullptr}, maxSize{0}, pos{0} {}
            BasicMapIterator (detail::MapPair<K, V>* _data, std::size_t _maxSize, std::size_t _pos = 0) : data{_data}, maxSize{_maxSize}, pos{_pos} {
                while (pos < maxSize && !data[pos].exists()) {
                    pos++;
                }
            }

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = K;
            using reference = const K&;

            reference operator* () const {
                return *data[pos].getKeyPtr();
            }

            BasicMapIterator<K, V>& operator++ () {
                bool first = true;
                while (pos < maxSize && (first || !data[pos].exists())) {
                    pos++;
                    first = false;
                }

                return *this;
            }

            BasicMapIterator<K, V> operator++ (int) {
                BasicMapIterator<K, V> other = *this;
                return ++other;
            }

            BasicMapIterator<K, V>& operator-- () {
                bool first = true;
                while (pos >= 0 && (first || !data[pos].exists())) {
                    pos--;
                    first = false;
                }

                return *this;
            }

            BasicMapIterator<K, V> operator-- (int) {
                BasicMapIterator<K, V> other = *this;
                return --other;
            }

            bool operator== (const BasicMapIterator<K, V>& other) const {
                return data == other.data && pos == other.pos;
            }
        };

        template <typename K, typename V, bool IsConst>
        class BasicMapKVIterator {
        private:
            detail::MapPair<K, V>* data;
            std::size_t pos;
            std::size_t maxSize;
        public:
            BasicMapKVIterator () : data{nullptr}, pos{0}, maxSize{0} {}
            BasicMapKVIterator (detail::MapPair<K, V>* _data, std::size_t _maxSize, std::size_t _pos = 0) : data{_data}, pos{_pos}, maxSize{_maxSize} {
                while (pos < maxSize && !data[pos].exists()) {
                    pos++;
                }
            }

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::pair<const K&, std::conditional_t<IsConst, const V&, V&>>;

            value_type operator* () const {
                return {*data[pos].getKeyPtr(), *data[pos].getValPtr()};
            }

            BasicMapKVIterator<K, V, IsConst>& operator++ () {
                bool first = true;
                while (pos < maxSize && (first || !data[pos].exists())) {
                    pos++;
                    first = false;
                }

                return *this;
            }

            BasicMapKVIterator<K, V, IsConst> operator++ (int) {
                BasicMapKVIterator<K, V, IsConst> other = *this;
                return ++other;
            }

            BasicMapKVIterator<K, V, IsConst>& operator-- () {
                bool first = true;
                while (pos >= 0 && (first || !data[pos].exists())) {
                    pos--;
                    first = false;
                }

                return *this;
            }

            BasicMapKVIterator<K, V, IsConst> operator-- (int) {
                BasicMapKVIterator<K, V, IsConst> other = *this;
                return --other;
            }

            bool operator== (const BasicMapKVIterator<K, V, IsConst>& other) const {
                return data == other.data && pos == other.pos;
            }

        };

        template <typename K, typename V, bool IsConst>
        class BasicMapKVView {
        private:
            detail::MapPair<K, V>* data;
            std::size_t maxSize;
        public:
            BasicMapKVView (detail::MapPair<K, V>* _data, std::size_t _maxSize) : data{_data}, maxSize{_maxSize} {}

            using iterator = BasicMapKVIterator<K, V, false>;
            using const_iterator = BasicMapKVIterator<K, V, true>;

            template <typename T = iterator>
            std::enable_if_t<!IsConst, T> begin () {
                return iterator(data, maxSize);
            }

            template <typename T = iterator>
            std::enable_if_t<!IsConst, T> end () {
                return iterator(data, maxSize, maxSize);
            }

            template <typename T = const_iterator>
            std::enable_if_t<IsConst, T> begin () const {
                return const_iterator(data, maxSize);
            }

            template <typename T = const_iterator>
            std::enable_if_t<IsConst, T> end () const {
                return const_iterator(data, maxSize, maxSize);
            }

            const_iterator cbegin () const {
                return const_iterator(data, maxSize);
            }

            const_iterator cend () const {
                return const_iterator(data, maxSize, maxSize);
            }
        };

    }

    template <typename K, typename V, typename HashFunc = std::hash<K>>
    class BasicMap {
    private:
        std::unique_ptr<detail::MapPair<K, V>[]> data;
        float loadFactor = 0.75f;
        std::size_t maxSize = 16;
        std::size_t currentSize = 0;

        std::size_t getHash (const K& key) const {
            HashFunc hashFunc;

            return hashFunc(key);
        }

        detail::MapPair<K, V>* getFirstPair (std::size_t hash) const {
            std::size_t index = hash % maxSize;

            detail::MapPair<K, V>* firstDeleted = nullptr;

            while (data[index].occupied()) {
                if (data[index].hash == hash) {
                    return &data[index];
                } else if (!firstDeleted && data[index].deleted()) {
                    firstDeleted = &data[index];
                }

                index = (index + 1) % maxSize;
            }

            return firstDeleted ? firstDeleted : &data[index];
        }

        detail::MapPair<K, V>* getValidPair (const K& key) const {
            auto pair = getFirstPair(getHash(key));

            return pair->exists() ? pair : nullptr;
        }

        detail::MapPair<K, V>* getOrCreatePair (const K& key) {
            std::size_t hash = getHash(key);
            auto pair = getFirstPair(hash);
            if (pair->exists()) {
                return pair;
            } else {
                pair->constructKey(hash, key);
                currentSize++;
                //resizeIfNecessary();
                return resizeIfNecessary() ? getFirstPair(hash) : pair;
            }
        }

        bool resizeIfNecessary () {
            if (currentSize <= (int)maxSize * loadFactor) {
                return false;
            }

            std::size_t newMaxSize = maxSize * 2;
            std::unique_ptr<detail::MapPair<K, V>[]> newData = std::make_unique<detail::MapPair<K, V>[]>(newMaxSize);

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

            return true;
        }

    public:
        using iterator = detail::BasicMapIterator<K, V>;
        using const_iterator = iterator;
        using kv_view = detail::BasicMapKVView<K, V, false>;
        using const_kv_view = detail::BasicMapKVView<K, V, true>;
        BasicMap () {
            data = std::make_unique<detail::MapPair<K, V>[]>(maxSize);
        }

        BasicMap (const BasicMap<K, V, HashFunc>& other) {
            maxSize = other.maxSize;
            currentSize = other.currentSize;
            data = std::make_unique<detail::MapPair<K, V>[]>(maxSize);

            for (std::size_t i = 0; i < other.maxSize; i++) {
                if (other.data[i].exists()) {
                    data[i] = other.data[i];
                }
            }
        }

        BasicMap (BasicMap<K, V, HashFunc>&& other) noexcept {
            maxSize = other.maxSize;
            currentSize = other.currentSize;
            data = std::move(other.data);
        }

        BasicMap<K, V, HashFunc>& operator= (const BasicMap<K, V, HashFunc>& other) {
            maxSize = other.maxSize;
            currentSize = other.currentSize;
            data = std::make_unique<detail::MapPair<K, V>[]>(maxSize);

            for (std::size_t i = 0; i < other.maxSize; i++) {
                if (other.data[i].exists()) {
                    data[i] = other.data[i];
                }
            }

            return *this;
        }

        BasicMap<K, V, HashFunc>& operator= (BasicMap<K, V, HashFunc>&& other) noexcept {
            maxSize = other.maxSize;
            currentSize = other.currentSize;
            data = std::move(other.data);

            return *this;
        }

        V& operator[] (const K& key) {
            auto pair = getOrCreatePair(key);

            return *pair->getValPtr();
        }

        const V& operator[] (const K& key) const {
            auto pair = getOrCreatePair(key);

            return *pair->getValPtr();
        }

        V& at (const K& key) {
            auto pair = getValidPair(key);

            if (!pair) {
                throw std::out_of_range("Key not in map!");
            }

            return *pair->getValPtr();
        }

        const V& at (const K& key) const {
            auto pair = getValidPair(key);

            if (!pair) {
                throw std::out_of_range("Key not in map!");
            }

            return *pair->getValPtr();
        }

        bool contains (const K& key) const {
            return getValidPair(key);
        }

        void remove (const K& key) {
            auto pair = getValidPair(key);

            if (pair) {
                pair->destructData();
                currentSize--;
            }
        }

        void clear () {
            for (std::size_t i = 0; i < maxSize; i++) {
                data[i].clear();
            }

            currentSize = 0;
        }

        template <typename ...Args>
        void emplace (const K& key, Args... args) {
            auto hash = getHash(key);
            auto pair = getFirstPair(hash);

            if (pair->exists()) {
                pair->constructValue(std::forward<Args>(args)...);
            } else {
                pair->constructKey(hash, key, std::forward<Args>(args)...);
            }
        }

        [[nodiscard]] std::size_t size () const {
            return currentSize;
        }

        [[nodiscard]] bool empty () const {
            return currentSize == 0;
        }

        iterator begin () {
            return iterator(data.get(), maxSize);
        }

        const_iterator begin () const {
            return const_iterator(data.get(), maxSize);
        }

        const_iterator cbegin () const {
            return const_iterator(data.get(), maxSize);
        }

        iterator end () {
            return iterator(data.get(), maxSize, maxSize);
        }

        const_iterator end () const {
            return const_iterator(data.get(), maxSize, maxSize);
        }

        const_iterator cend() const {
            return const_iterator(data.get(), maxSize, maxSize);
        }

        detail::BasicMapKVView<K, V, false> kv () {
            return detail::BasicMapKVView<K, V, false>(data.get(), maxSize);
        }

        detail::BasicMapKVView<K, V, true> kv () const {
            return detail::BasicMapKVView<K, V, true>(data.get(), maxSize);
        }


        /*std::string toString () {
            std::stringstream outStream;

            outStream << "{\n";
            for (auto [k, v] : kv()) {
                outStream << "  " << k << " : " << v << "\n";
            }
            outStream << "}";

            return outStream.str();
        }*/
    };

    // This specifically exists because c++ standard is stupid
    // Not thread safe
    template <typename T>
    class ValueWrapper;

    template <typename K, typename V, typename Wrap = ValueWrapper<V>>
    class MapIterator;

    template <typename K, typename V, typename hashFunc=std::hash<K>>
    class Map2 {
    protected:
        // 0 = no item, 1 = has item, -1 = deleted (sentinel)
        std::unique_ptr<std::pair<char, K>[]> exists;
        std::unique_ptr<unsigned char[]> data;
        float loadFactor = 0.75;
        int maxSize = 16;
        int currentSize = 0;

        /*std::pair<char, K>* getExists () {
            return (std::pair<char, K>*)exists.get();
        }*/

        V* getData () const {
            return (V*)data.get();
        }

        void resizeContainer () {

            std::unique_ptr<std::pair<char, K>[]> newExists(std::unique_ptr<std::pair<char, K>[]>(new std::pair<char, K>[maxSize * 2]));
            //auto newExistsPtr = (std::pair<char, K>*)newExists.get();
            //std::unique_ptr<V[]> newData = std::make_unique<V[maxSize * 2]>();
            std::unique_ptr<unsigned char[]> newData( new unsigned char[maxSize * 2 * sizeof(V)]);
            auto newDataPtr = (V*)newData.get();
            for (int i = 0; i < maxSize * 2; i++) {
                newExists[i].first = 0;
            }

            for (int i = 0; i < maxSize; i++) {
                if (exists[i].first == 1) {
                    auto hash = hashFunc{}(exists[i].second) % (maxSize * 2);
                    while (newExists[hash].first == 1) {
                        hash = (hash + 1) % (maxSize * 2);
                    }
                    newExists[hash].first = 1;
                    newExists[hash].second = std::move(exists[i].second);
                    new (newDataPtr + hash) V(std::move(getData()[i]));
                    getData()[i].~V();
                }
            }
            maxSize *= 2;
            exists = std::move(newExists);
            data = std::move(newData);
        }

        std::size_t findPos (const K& key, std::size_t hash) const {
            auto i = hash % maxSize;
            std::size_t firstSentinel = 0;
            bool hitSentinel = false;
            while (exists[i].first == -1 || (exists[i].first == 1 && exists[i].second != key)) {
                if (!hitSentinel && exists[i].first == -1) {
                    hitSentinel = true;
                    firstSentinel = i;
                }
                i = (i + 1) % maxSize;
            }
            if (hitSentinel && exists[i].first == 0) {
                i = firstSentinel;
            }
            return i;
        }

        template <typename ...Args>
        std::size_t emplaceElement (std::size_t i, const K& key, std::size_t hash, Args... args) {
            currentSize++;
            if (maxSize * loadFactor < currentSize) {
                resizeContainer();
                i = hash % maxSize;
                while (exists[i].first == 1) {
                    i = (i + 1) % maxSize;
                }
            }
            auto ptr = getData() + i;
            new (ptr) V(args...);
            exists[i].first = 1;
            exists[i].second = key;
            //size++;
            return i;
        }

        std::size_t getOrNew (const K& key) {
            auto hash = hashFunc{}(key);

            auto pos = findPos(key, hash);


            if (exists[pos].first != 1) {
                pos = emplaceElement(pos, key, hash);
            }
            return pos;
        }

        void removeElement (const K& key, std::size_t hash) {
            auto pos = findPos(key, hash);
            if (exists[pos].first == 1) {
                exists[pos].first = -1;
                exists[pos].second = K();
                getData()[pos].~V();
                currentSize--;
            } else {
                throw std::out_of_range("Key does not exist in map!");
            }
        }

        V& getFromKey (const K& key) {
            auto pos = getOrNew(key);
            return getData()[pos];
        }
        bool containsInternal (const K& key) const{
            return exists[findPos(key, hashFunc{}(key))].first == 1;
        }

        void removeInternal (const K& key) {
            removeElement(key, hashFunc{}(key));
        }

        V& atKey (const K& key) const {
            auto hash = hashFunc{}(key);
            auto pos = findPos(key, hash);
            if (exists[pos].first != 1) {
                throw std::out_of_range("Key not in map!");
            }
            return getData()[pos];
        }

    public:
        using iterator = MapIterator<K, V>;
        using const_iterator = MapIterator<K, V, const ValueWrapper<V>>;
        Map2 () {
            exists = std::make_unique<std::pair<char, K>[]>(maxSize);
            for (int i = 0; i < maxSize; i++) {
                exists[i].first = 0;
            }
            data = std::move(std::unique_ptr<unsigned char[]>(new unsigned char[maxSize * sizeof(V)]));
        }

        Map2 (const Map2<K, V, hashFunc>& other) : loadFactor(other.loadFactor), maxSize(other.maxSize), currentSize(other.currentSize),
                                                 exists(std::make_unique<std::pair<char, K>[]>(other.maxSize)),
                                                 data(std::make_unique<unsigned char[]>(other.maxSize * sizeof(V))) {
            auto otherData = (V*)other.data.get();
            for (int i = 0; i < maxSize; i++) {
                exists[i] = other.exists[i];
                if (exists[i].first == 1) {
                    new (getData() + i) V(otherData[i]);
                }
            }
        }

        Map2<K, V, hashFunc>& operator= (const Map2<K, V, hashFunc>& other) {
            if (&other != this) {
                loadFactor = other.loadFactor;

                if (maxSize != other.maxSize) {
                    exists = std::make_unique<std::pair<char, K>[]>(other.maxSize);
                    data = std::make_unique<unsigned char[]>(other.maxSize * sizeof(V));
                }

                maxSize = other.maxSize;
                currentSize = other.currentSize;

                auto otherData = (V*)other.data.get();

                for (int i = 0; i < maxSize; i++) {
                    exists[i] = other.exists[i];
                    if (exists[i].first == 1) {
                        new(getData() + i) V(otherData[i]);
                    }
                }
            }
            return *this;
        }

        Map2(Map2<K, V, hashFunc>&&) = default;
        Map2<K, V, hashFunc>& operator= (Map2<K, V, hashFunc>&&) = default;

        template <typename T>
        V operator[] (const T& key) const {
            //auto pos = getOrNew(key);
            //return getData()[pos];
            return getFromKey(std::forward<const T>(key));
        }

        /*V operator[] (K&& key) const {
            auto pos = getOrNew(key);
            return getData()[pos];
        }*/

        template <typename T>
        V& operator[] (const T& key) {
            //auto pos = getOrNew(key);
            //return getData()[pos];
            return getFromKey(std::forward<const T>(key));
        }

        template <typename T>
        V& at (const T& key) {
            return atKey(std::forward<const T>(key));
        }

        template <typename T>
        V const& at (const T& key) const {
            return atKey(std::forward<const T>(key));
        }

        /*V& operator[] (K&& key) {
            auto pos = getOrNew(key);
            return getData()[pos];
        }*/

        template <typename T>
        bool contains (const T& key) const {
            return containsInternal(std::forward<const T>(key));
        }

        template <typename T>
        void remove (const T& key) {
            //removeElement(key, hashFunc{}(key));
            removeInternal(std::forward<const T>(key));
        }

        std::size_t size () const {
            return currentSize;
        }

        bool empty () const {
            return currentSize == 0;
        }

        ~Map2 () {
            if (exists && data) {
                for (int i = 0; i < maxSize; i++) {
                    if (exists[i].first == 1) {
                        getData()[i].~V();
                    }
                }
            }
        }

        iterator begin () {
            return iterator(*this, 0);
        }

        const_iterator begin () const {
            return cbegin();
        }

        const_iterator cbegin() const {
            return const_iterator(*this, 0);
        }

        iterator end () {
            return iterator(*this, maxSize);
        }

        const_iterator end () const {
            return cend();
        }

        const_iterator cend () const {
            return const_iterator (*this, maxSize);
        }

        friend  MapIterator<K, V>;
        friend  MapIterator<K, V, const ValueWrapper<V>>;
    };



    template <typename T>
    class ValueWrapper {
    private:
        T* ptr = nullptr;
    public:
        ValueWrapper() = default;
        ValueWrapper(T* _ptr) : ptr(_ptr){}
        ValueWrapper& operator=(T t) {
            *ptr = t;
        }
        operator T& () {
            return *ptr;
        }

        operator const T& () const {
            return *ptr;
        }

        template <typename I>
        auto operator+=(I i) -> decltype(*ptr += i) {
            return *ptr += i;
        }
        template <typename I>
        auto operator-=(I i) -> decltype(*ptr -= i) {
            return *ptr -= i;
        }
        template <typename I>
        auto operator*=(I i) -> decltype(*ptr *= i) {
            return *ptr *= i;
        }
        template <typename I>
        auto operator/=(I i) -> decltype(*ptr /= i) {
            return *ptr /= i;
        }
        T& operator() () {
            return *ptr;
        }

        T const& operator() () const {
            return *ptr;
        }
    };



    template <typename K, typename V, typename Wrapper>
    class MapIterator {
    private:

        using Wt = meta::remove_const_if_exist<Wrapper>;
        std::pair<char, K>* exists = nullptr;
        V* data = nullptr;
        int maxSize = 0;
        std::size_t index = 0;
        mutable std::pair<K, Wt> p{};

        void nextIndex () {
            while (index < maxSize && exists[index].first != 1) {
                index++;
            }
        }

        void getPair () const {
            if (index >= maxSize) {
                throw std::exception();
            }

            p.first = exists[index].second;
            p.second = Wt(data + index);
        }

    public:
        using iterator = MapIterator<K, V, Wrapper>;
        using value_type = std::pair<const K, V>;
        using reference = std::pair<const K, Wrapper>&;
        using pointer = std::pair<const K, Wrapper>*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        // Iterator
        MapIterator (const MapIterator<K, V, Wrapper>& it) : exists(it.exists), data(it.data), maxSize(it.maxSize), index(it.index) {}
        /*MapIterator& operator= (const iterator& other) {
            exists = other.exists;
            data = other.data;
            maxSize = other.maxSize;
            index = other.index;
            return *this;
        }
        MapIterator& operator= (iterator&& other) noexcept {
            exists = std::move(other.exists);
            data = std::move(other.data);
            maxSize = std::move(other.maxSize);
            index = std::move(other.index);
            p = std::move(other.p);
        }*/
        // prefix
        iterator& operator++ () {
            index++;
            nextIndex();
            return *this;
        }
        reference operator* () const {
            getPair();
            return (reference)p;
        }
        ~MapIterator () = default;

        // Input / Output Iterator
        // postfix
        iterator operator++ (int) {
            auto cpy(*this);
            index++;
            nextIndex();
            return cpy;
        }
        //value_type  operator*() const;
        pointer operator-> () {
            getPair();
            return &p;
        };
        friend bool operator== (const iterator& it1, const iterator& it2) {
            return (it1.index >= it1.maxSize && it2.index >= it2.maxSize) || (it1.data == it2.data && it1.index == it2.index);
        };
        friend bool operator!= (const iterator& it1, const iterator& it2) {
            return !(it1 == it2);
        };
        MapIterator () = default;

        template <typename HashFunc>
        MapIterator(const Map2<K, V, HashFunc>& map, std::size_t startIndex) : maxSize{map.maxSize}, index{startIndex}, exists(map.exists.get()), data{(V*)map.data.get()} {
            nextIndex();
        };
    };

    template <typename K, typename V, typename HashFunc = std::hash<K>>
    using Map = BasicMap<K, V, HashFunc>;
}
