#include "logging/logging.h"
#include "util/map.h"
#include "util/set.h"

#include <cassert>
#include <iterator>
#include <string>
#include <unordered_set>

using namespace phenyl::util;

static void testSetEmpty () {
    Set<std::string> set;

    PHENYL_DASSERT(set.empty());

    set.insert("Hello");

    PHENYL_DASSERT(!set.empty());

    set.remove("Hello");

    PHENYL_DASSERT(set.empty());
}

static void testSetContains () {
    Set<std::string> set;

    PHENYL_DASSERT(!set.contains("Hello"));
    PHENYL_DASSERT(!set["Hello"]);

    set.insert("Hello");

    PHENYL_DASSERT(set.contains("Hello"));
    PHENYL_DASSERT(set["Hello"]);

    PHENYL_DASSERT(!set.contains("Goodbye"));
    PHENYL_DASSERT(!set["Goodbye"]);

    set.insert("Goodbye");

    PHENYL_DASSERT(set["Hello"]);
    PHENYL_DASSERT(set["Goodbye"]);

    set.remove("Hello");

    PHENYL_DASSERT(!set["Hello"]);
    PHENYL_DASSERT(set["Goodbye"]);
}

static void testSetSize () {
    Set<std::string> set;

    PHENYL_DASSERT(set.size() == 0);

    set.insert("test1");
    PHENYL_DASSERT(set.size() == 1);
    set.insert("test2");
    PHENYL_DASSERT(set.size() == 2);
    set.insert("test3");
    PHENYL_DASSERT(set.size() == 3);

    set.remove("test3");
    PHENYL_DASSERT(set.size() == 2);
    set.remove("test2");
    PHENYL_DASSERT(set.size() == 1);
    set.remove("test1");
    PHENYL_DASSERT(set.size() == 0);
}

static void testSetClear () {
    Set<std::string> set;

    set.insert("test1");
    set.insert("test2");
    set.insert("test3");
    set.insert("test4");
    set.insert("test5");
    set.insert("test6");

    PHENYL_DASSERT(set.size() == 6);

    set.clear();

    PHENYL_DASSERT(!set.contains("test1"));
    PHENYL_DASSERT(!set.contains("test2"));
    PHENYL_DASSERT(!set.contains("test3"));
    PHENYL_DASSERT(!set.contains("test4"));
    PHENYL_DASSERT(!set.contains("test5"));
    PHENYL_DASSERT(!set.contains("test6"));
    PHENYL_DASSERT(set.size() == 0);
}

void testSetLarge () {
    Set<std::string> set;

    for (int i = 0; i < 1000; i++) {
        set.insert("test" + std::to_string(i));
    }

    PHENYL_DASSERT(set.size() == 1000);

    for (int i = 0; i < 1000; i++) {
        PHENYL_DASSERT(set["test" + std::to_string(i)]);
    }

    for (int i = 0; i < 1000; i += 3) {
        set.remove("test" + std::to_string(i));
    }

    for (int i = 0; i < 1000; i += 3) {
        PHENYL_DASSERT(!set.contains("test" + std::to_string(i)));
    }

    PHENYL_DASSERT(set.size() == 1000 - 334);

    for (int i = 0; i < 500; i++) {
        set.insert("abc" + std::to_string(i));
    }

    PHENYL_DASSERT(set.size() == 1000 - 334 + 500);

    for (int i = 0; i < 1000; i++) {
        if (i % 3 == 0) {
            PHENYL_DASSERT(!set["test" + std::to_string(i)]);
        } else {
            PHENYL_DASSERT(set["test" + std::to_string(i)]);
        }
    }

    for (int i = 0; i < 500; i++) {
        PHENYL_DASSERT(set["abc" + std::to_string(i)]);
    }

    for (int i = 0; i < 1000; i++) {
        set.remove("testing" + std::to_string(i));
    }

    PHENYL_DASSERT(set.size() == 1000 - 334 + 500);

    for (int i = 0; i < 1000; i++) {
        if (i % 3 == 0) {
            PHENYL_DASSERT(!set["test" + std::to_string(i)]);
        } else {
            PHENYL_DASSERT(set["test" + std::to_string(i)]);
        }
    }

    for (int i = 0; i < 500; i++) {
        PHENYL_DASSERT(set["abc" + std::to_string(i)]);
    }

    std::unordered_set<std::string> vals;

    for (auto& i : set) {
        vals.insert(i);
    }

    PHENYL_DASSERT(vals.size() == set.size());

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
