#include <functional>
#include <memory>
#include <utility>
#include <cstring>
#include <cstdint>
#ifndef UTIL_MAP_H
#define UTIL_MAP_H
namespace util {
    // This specifically exists because c++ standard is stupid
    // Not thread safe
    template <typename T>
    class ValueWrapper;

    template <typename K, typename V, typename Wrap = ValueWrapper<V>>
    class MapIterator;

    template <typename K, typename V, typename hashFunc=std::hash<K>>
    class Map {
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
        bool containsInternal (const K& key) {
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
        Map () {
            exists = std::make_unique<std::pair<char, K>[]>(maxSize);
            for (int i = 0; i < maxSize; i++) {
                exists[i].first = 0;
            }
            data = std::move(std::unique_ptr<unsigned char[]>(new unsigned char[maxSize * sizeof(V)]));
        }

        Map (const Map<K, V, hashFunc>& other) : loadFactor(other.loadFactor), maxSize(other.maxSize), currentSize(other.currentSize),
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

        Map<K, V, hashFunc>& operator= (const Map<K, V, hashFunc>& other) {
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

        Map(Map<K, V, hashFunc>&&) = default;
        Map<K, V, hashFunc>& operator= (Map<K, V, hashFunc>&&) = default;

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
        bool contains (const T& key) {
            return containsInternal(std::forward<const T>(key));
        }

        template <typename T>
        void remove (const T& key) {
            //removeElement(key, hashFunc{}(key));
            removeInternal(std::forward<const T>(key));
        }

        std::size_t size () {
            return currentSize;
        }

        ~Map () {
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
        std::pair<K, Wt> p{};

        void nextIndex () {
            while (index < maxSize && exists[index].first != 1) {
                index++;
            }
        }

        void getPair () {
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
        MapIterator& operator= (iterator& other) {
            exists = other.exists;
            data = other.data;
            maxSize = other.maxSize;
            index = other.index;
            return *this;
        }
        // prefix
        iterator& operator++ () {
            index++;
            nextIndex();
            return *this;
        }
        reference operator* () {
            getPair();
            return (std::pair<const K, Wrapper>&)p;
        }
        ~MapIterator () = default;

        // Input / Output Iterator
        // postfix
        const iterator operator++ (int) {
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
        MapIterator(const Map<K, V, HashFunc>& map, std::size_t startIndex) : maxSize{map.maxSize}, index{startIndex}, exists(map.exists.get()), data{(V*)map.data.get()} {
            nextIndex();
        };
    };

}
#endif
