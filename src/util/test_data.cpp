#include <assert.h>

#include "data.h"

#include "logging/logging.h"

using namespace util;

struct DestructorTest {
    std::shared_ptr<int> ptr{};
    DestructorTest() = default;
    explicit DestructorTest(std::shared_ptr<int> _ptr) : ptr(std::move(_ptr)) {
        (*ptr)++;
    }

    DestructorTest& operator= (DestructorTest const& other) {
        if (&other != this) {
            ptr = other.ptr;
            (*ptr)++;
        }
        return *this;
    }

    DestructorTest (DestructorTest const& other) {
        ptr = other.ptr;
        (*ptr)++;
    }

    DestructorTest (DestructorTest&& other) = default;

    ~DestructorTest() {
        if (ptr) {
            (*ptr)--;
        }
    }
};

void testDeletion (std::shared_ptr<int> ptr) {
    util::Map<int, DestructorTest> map;
    assert(*ptr == 0);

    map[-1] = DestructorTest(ptr);

    assert(*ptr == 1);

    for (int i = 0; i < 48; i++) {
        map[i] = DestructorTest(ptr);
    }

    logging::log(LEVEL_DEBUG, "Size: {}", *ptr);
    assert((*ptr) == 49);

    auto mapCpy(map);
    logging::log(LEVEL_DEBUG, "Size: {}", *ptr);
    assert((*ptr) == 98);

    auto mapMv(std::move(mapCpy));
    logging::log(LEVEL_DEBUG, "Size: {}", *ptr);
    assert((*ptr) == 98);

    for (int i = 1; i < 22; i += 2) {
        map.remove(i);
    }
    logging::log(LEVEL_DEBUG, "Size: {}", *ptr);
    assert((*ptr) == 87);
}

void testCopyMove () {
    util::Map<std::string, int> map;
    map["hello"] = 1;
    map["world"] = 2;
    map[":)"] = 3;
    util::Map<std::string, int> mapCpy1(map);
    util::Map<std::string, int> mapCpyEq;
    mapCpyEq = map;

    assert(mapCpy1.size() == map.size());
    assert(mapCpyEq.size() == map.size());

    assert(mapCpy1["hello"] == 1);
    assert(mapCpy1["world"] == 2);
    assert(mapCpy1[":)"] == 3);

    assert(mapCpyEq["hello"] == 1);
    assert(mapCpyEq["world"] == 2);
    assert(mapCpyEq[":)"] == 3);

    map["hello"] = 4;

    assert(mapCpy1["hello"] == 1);
    assert(mapCpyEq["hello"] == 1);

    for (int i = 0; i < 11; i++) {
        map["test" + std::to_string(i)] = i * i;
    }

    auto mapCpy2(map);
    mapCpyEq = map;

    assert(mapCpy2.size() == map.size());
    assert(mapCpyEq.size() == map.size());

    assert(mapCpy2["hello"] == 4);
    assert(mapCpy2["world"] == 2);
    assert(mapCpy2[":)"] == 3);

    assert(mapCpyEq["hello"] == 4);
    assert(mapCpyEq["world"] == 2);
    assert(mapCpyEq[":)"] == 3);

    for (int i = 0; i < 11; i++) {
        mapCpy2["test" + std::to_string(i)] = i * i;
        mapCpyEq["test" + std::to_string(i)] = i * i;
    }

    auto mapMv(std::move(mapCpy2));
    util::Map<std::string, int> mapMvEq;
    mapMvEq = std::move(mapCpyEq);

    assert(mapMv.size() == map.size());
    assert(mapMvEq.size() == map.size());

    assert(mapMv["hello"] == 4);
    assert(mapMv["world"] == 2);
    assert(mapMv[":)"] == 3);

    assert(mapMvEq["hello"] == 4);
    assert(mapMvEq["world"] == 2);
    assert(mapMvEq[":)"] == 3);

    for (int i = 0; i < 11; i++) {
        mapMv["test" + std::to_string(i)] = i * i;
        mapMvEq["test" + std::to_string(i)] = i * i;
    }

    for (auto& i : map) {
        assert(map.contains(i.first));
        assert(map[i.first] == i.second);
    }

}

void testMap () {
    logging::log(LEVEL_DEBUG, "Testing map!");

    util::Map<std::string, int> map;

    assert(map.size() == 0);

    map["hello"] = 4;

    assert(map["hello"] == 4);

    map["world"] = 3;

    assert(map["hello"] == 4);
    assert(map["world"] == 3);

    map["hello"] = 10;

    assert(map["hello"] == 10);
    assert(map["world"] == 3);

    assert(map.contains("hello"));
    assert(map.contains("world"));
    assert(!map.contains("cherry"));

    assert(map.size() == 2);

    for (int i = 0; i < 11; i++) {
        map["test" + std::to_string(i)] = i * i;
    }

    assert(map.size() == 13);

    assert(map.contains("hello"));
    assert(map["hello"] == 10);
    assert(map.contains("world"));
    assert(map["world"] == 3);

    for (int i = 0; i < 11; i++) {
        assert(map.contains("test" + std::to_string(i)));
        //logging::log(LEVEL_DEBUG, "Key: {}, val: {}", "test" + std::to_string(i), map["test" + std::to_string(i)]);
        assert(map["test" + std::to_string(i)] == i * i);
    }

    map.remove("world");

    assert(map.size() == 12);
    assert(map.contains("hello"));
    assert(!map.contains("world"));
    assert(map["hello"] == 10);

    for (int i = 0; i < 11; i++) {
        assert(map.contains("test" + std::to_string(i)));
        assert(map["test" + std::to_string(i)] == i * i);
    }

    auto ptr = std::make_shared<int>(0);

    testCopyMove();

    testDeletion(ptr);

    assert((*ptr) == 0);

    logging::log(LEVEL_DEBUG, "Assertions for map passed!");

}


void util::testData () {
    testMap();
}