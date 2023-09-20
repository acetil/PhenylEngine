#include "util/set.h"
#include "util/map.h"
#include "logging/logging.h"
#include <cassert>
#include <string>
#include <iterator>
#include <unordered_set>

using namespace phenyl::util;

static void testSetEmpty () {
    Set<std::string> set;

    assert(set.empty());

    set.insert("Hello");

    assert(!set.empty());

    set.remove("Hello");

    assert(set.empty());
}

static void testSetContains () {
    Set<std::string> set;

    assert(!set.contains("Hello"));
    assert(!set["Hello"]);

    set.insert("Hello");

    assert(set.contains("Hello"));
    assert(set["Hello"]);

    assert(!set.contains("Goodbye"));
    assert(!set["Goodbye"]);

    set.insert("Goodbye");

    assert(set["Hello"]);
    assert(set["Goodbye"]);

    set.remove("Hello");

    assert(!set["Hello"]);
    assert(set["Goodbye"]);
}

static void testSetSize () {
    Set<std::string> set;

    assert(set.size() == 0);

    set.insert("test1");
    assert(set.size() == 1);
    set.insert("test2");
    assert(set.size() == 2);
    set.insert("test3");
    assert(set.size() == 3);

    set.remove("test3");
    assert(set.size() == 2);
    set.remove("test2");
    assert(set.size() == 1);
    set.remove("test1");
    assert(set.size() == 0);
}

static void testSetClear () {
    Set<std::string> set;

    set.insert("test1");
    set.insert("test2");
    set.insert("test3");
    set.insert("test4");
    set.insert("test5");
    set.insert("test6");

    assert(set.size() == 6);

    set.clear();

    assert(!set.contains("test1"));
    assert(!set.contains("test2"));
    assert(!set.contains("test3"));
    assert(!set.contains("test4"));
    assert(!set.contains("test5"));
    assert(!set.contains("test6"));
    assert(set.size() == 0);
}

void testSetLarge () {
    Set<std::string> set;

    for (int i = 0; i < 1000; i++) {
        set.insert("test" + std::to_string(i));
    }

    assert(set.size() == 1000);

    for (int i = 0; i < 1000; i++) {
        assert(set["test" + std::to_string(i)]);
    }

    for (int i = 0; i < 1000; i += 3) {
        set.remove("test" + std::to_string(i));
    }

    for (int i = 0; i < 1000; i += 3) {
        assert(!set.contains("test" + std::to_string(i)));
    }

    assert(set.size() == 1000 - 334);

    for (int i = 0; i < 500; i++) {
        set.insert("abc" + std::to_string(i));
    }

    assert(set.size() == 1000 - 334 + 500);

    for (int i = 0; i < 1000; i++) {
        if (i % 3 == 0) {
            assert(!set["test" + std::to_string(i)]);
        } else {
            assert(set["test" + std::to_string(i)]);
        }
    }

    for (int i = 0; i < 500; i++) {
        assert(set["abc" + std::to_string(i)]);
    }

    for (int i = 0; i < 1000; i++) {
        set.remove("testing" + std::to_string(i));
    }

    assert(set.size() == 1000 - 334 + 500);

    for (int i = 0; i < 1000; i++) {
        if (i % 3 == 0) {
            assert(!set["test" + std::to_string(i)]);
        } else {
            assert(set["test" + std::to_string(i)]);
        }
    }

    for (int i = 0; i < 500; i++) {
        assert(set["abc" + std::to_string(i)]);
    }

    std::unordered_set<std::string> vals;

    for (auto& i : set) {
        vals.insert(i);
    }

    assert(vals.size() == set.size());

    auto set2 = set;
    auto set3 = std::move(set2);
    Set<std::string> set4;
    set4 = set3;
    Set<std::string> set5;
    set5 = std::move(set4);
    set5.insert("hello");
}

void phenyl::util::setTest () {
    testSetEmpty();
    testSetContains();
    testSetSize();
    testSetClear();
    testSetLarge();
    static_assert(std::bidirectional_iterator<Set<std::string>::iterator>);
    static_assert(std::bidirectional_iterator<detail::BasicMapKVView<std::string, long, true>::iterator>);
}