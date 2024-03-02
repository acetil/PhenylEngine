#include "util/data.h"

#include "logging/logging.h"

using namespace phenyl::util;

phenyl::Logger LOGGER{"UTIL_TEST"};

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
    Map<int, DestructorTest> map;
    PHENYL_DASSERT(*ptr == 0);

    map[-1] = DestructorTest(ptr);

    PHENYL_DASSERT(*ptr == 1);

    for (int i = 0; i < 48; i++) {
        map[i] = DestructorTest(ptr);
    }

    PHENYL_LOGD(LOGGER, "Size: {}", *ptr);
    PHENYL_DASSERT((*ptr) == 49);

    auto mapCpy(map);
    PHENYL_LOGD(LOGGER, "Size: {}", *ptr);
    PHENYL_DASSERT((*ptr) == 98);

    auto mapMv(std::move(mapCpy));
    PHENYL_LOGD(LOGGER, "Size: {}", *ptr);
    PHENYL_DASSERT((*ptr) == 98);

    for (int i = 1; i < 22; i += 2) {
        map.remove(i);
    }
    PHENYL_LOGD(LOGGER, "Size: {}", *ptr);
    PHENYL_DASSERT((*ptr) == 87);
}

void testCopyMove () {
    Map<std::string, int> map;
    map["hello"] = 1;
    map["world"] = 2;
    map[":)"] = 3;
    Map<std::string, int> mapCpy1(map);
    Map<std::string, int> mapCpyEq;
    mapCpyEq = map;

    PHENYL_DASSERT(mapCpy1.size() == map.size());
    PHENYL_DASSERT(mapCpyEq.size() == map.size());

    PHENYL_DASSERT(mapCpy1["hello"] == 1);
    PHENYL_DASSERT(mapCpy1["world"] == 2);
    PHENYL_DASSERT(mapCpy1[":)"] == 3);

    PHENYL_DASSERT(mapCpyEq["hello"] == 1);
    PHENYL_DASSERT(mapCpyEq["world"] == 2);
    PHENYL_DASSERT(mapCpyEq[":)"] == 3);

    map["hello"] = 4;

    PHENYL_DASSERT(mapCpy1["hello"] == 1);
    PHENYL_DASSERT(mapCpyEq["hello"] == 1);

    for (int i = 0; i < 11; i++) {
        map["test" + std::to_string(i)] = i * i;
    }

    auto mapCpy2(map);
    mapCpyEq = map;

    PHENYL_DASSERT(mapCpy2.size() == map.size());
    PHENYL_DASSERT(mapCpyEq.size() == map.size());

    PHENYL_DASSERT(mapCpy2["hello"] == 4);
    PHENYL_DASSERT(mapCpy2["world"] == 2);
    PHENYL_DASSERT(mapCpy2[":)"] == 3);

    PHENYL_DASSERT(mapCpyEq["hello"] == 4);
    PHENYL_DASSERT(mapCpyEq["world"] == 2);
    PHENYL_DASSERT(mapCpyEq[":)"] == 3);

    for (int i = 0; i < 11; i++) {
        mapCpy2["test" + std::to_string(i)] = i * i;
        mapCpyEq["test" + std::to_string(i)] = i * i;
    }

    auto mapMv(std::move(mapCpy2));
    Map<std::string, int> mapMvEq;
    mapMvEq = std::move(mapCpyEq);

    PHENYL_DASSERT(mapMv.size() == map.size());
    PHENYL_DASSERT(mapMvEq.size() == map.size());

    PHENYL_DASSERT(mapMv["hello"] == 4);
    PHENYL_DASSERT(mapMv["world"] == 2);
    PHENYL_DASSERT(mapMv[":)"] == 3);

    PHENYL_DASSERT(mapMvEq["hello"] == 4);
    PHENYL_DASSERT(mapMvEq["world"] == 2);
    PHENYL_DASSERT(mapMvEq[":)"] == 3);

    for (int i = 0; i < 11; i++) {
        mapMv["test" + std::to_string(i)] = i * i;
        mapMvEq["test" + std::to_string(i)] = i * i;
    }

    for (auto i : map.kv()) {
        PHENYL_DASSERT(map.contains(i.first));
        PHENYL_DASSERT(map[i.first] == i.second);
    }

}

void testMap () {
    PHENYL_LOGD(LOGGER, "Testing map!");

    Map<std::string, int> map;

    PHENYL_DASSERT(map.size() == 0);

    map["hello"] = 4;

    PHENYL_DASSERT(map["hello"] == 4);

    map["world"] = 3;

    PHENYL_DASSERT(map["hello"] == 4);
    PHENYL_DASSERT(map["world"] == 3);

    map["hello"] = 10;

    PHENYL_DASSERT(map["hello"] == 10);
    PHENYL_DASSERT(map["world"] == 3);

    PHENYL_DASSERT(map.contains("hello"));
    PHENYL_DASSERT(map.contains("world"));
    PHENYL_DASSERT(!map.contains("cherry"));

    PHENYL_DASSERT(map.size() == 2);

    for (int i = 0; i < 11; i++) {
        map["test" + std::to_string(i)] = i * i;
    }

    PHENYL_DASSERT(map.size() == 13);

    PHENYL_DASSERT(map.contains("hello"));
    PHENYL_DASSERT(map["hello"] == 10);
    PHENYL_DASSERT(map.contains("world"));
    PHENYL_DASSERT(map["world"] == 3);

    for (int i = 0; i < 11; i++) {
        PHENYL_DASSERT(map.contains("test" + std::to_string(i)));
        //logging::log(LEVEL_DEBUG, "Key: {}, val: {}", "test" + std::to_string(i), map["test" + std::to_string(i)]);
        PHENYL_DASSERT(map["test" + std::to_string(i)] == i * i);
    }

    map.remove("world");

    PHENYL_DASSERT(map.size() == 12);
    PHENYL_DASSERT(map.contains("hello"));
    PHENYL_DASSERT(!map.contains("world"));
    PHENYL_DASSERT(map["hello"] == 10);

    for (int i = 0; i < 11; i++) {
        PHENYL_DASSERT(map.contains("test" + std::to_string(i)));
        PHENYL_DASSERT(map["test" + std::to_string(i)] == i * i);
    }

    auto ptr = std::make_shared<int>(0);

    testCopyMove();

    testDeletion(ptr);

    PHENYL_DASSERT((*ptr) == 0);

    PHENYL_LOGD(LOGGER, "Assertions for map passed!");

}

void testDataTypes () {
    DataValue val;
    //PHENYL_DASSERT(val == std::monostate());
    val = 4;
    int v = (int)val;
    PHENYL_DASSERT (v == 4);
    bool threw = false;
    try {
        float f = (float)val;
    } catch (std::exception&) {
        threw = true;
    }
    PHENYL_DASSERT(threw);
    val = 4.5f;
    float f;
    PHENYL_DASSERT(val.getValue(f));
    PHENYL_DASSERT(f == 4.5f);

    val = std::string("Hello World!");

    std::string s = (std::string)val;
    PHENYL_DASSERT(s == "Hello World!");

    DataObject obj;

    obj["test1"] = 1;
    obj["test2"] = 3.0f;
    obj["test3"] = true;
    obj["test4"] = val;

    PHENYL_DASSERT(obj.contains("test1"));
    PHENYL_DASSERT(obj.contains("test2"));
    PHENYL_DASSERT(obj.contains("test3"));
    PHENYL_DASSERT(obj.contains("test4"));

    int t1 = (int)obj["test1"];
    float t2 = (float)obj["test2"];
    bool t3 = (bool)obj["test3"];
    std::string t4 = (std::string)obj["test4"];
    PHENYL_DASSERT(t1 == 1);
    PHENYL_DASSERT(t2 == 3.0f);
    PHENYL_DASSERT(t3);
    PHENYL_DASSERT(t4 == "Hello World!");

    DataArray arr;

    PHENYL_DASSERT(arr.empty());
    PHENYL_DASSERT(arr.size() == 0);

    arr.push_back(4);
    arr.push_back(4.5f);

    PHENYL_DASSERT(!arr.empty());
    PHENYL_DASSERT(arr.size() == 2);
    v = (int)arr[0];
    f = (float)arr[1];

    PHENYL_DASSERT(v == 4);
    PHENYL_DASSERT(f == 4.5f);

    val = 4;

    //auto eq = Operations::eq(v);

    //PHENYL_DASSERT(eq(4));

    PHENYL_DASSERT(val == 4);

    PHENYL_DASSERT(val > 1);

    PHENYL_DASSERT(val >= 4.0);
    PHENYL_DASSERT(val >= 3);

    PHENYL_DASSERT(val < 13.7);
    PHENYL_DASSERT(val <= 4.1);
    PHENYL_DASSERT(val <= 4);

    PHENYL_DASSERT(val != false);

    auto s1 = val.toString();
    PHENYL_DASSERT(s1 == "4");
}

void testJson () {
    auto jsonStr = R"({"hello" : 1, "world" : true, "test" : {"meme" : "lol", "testing" : 1.1}, "test2" : [4, 3, 2, 1, {}]})";
    auto val = parseJson(jsonStr);

    DataObject obj = (DataObject)val;

    PHENYL_DASSERT(obj.contains("hello"));
    PHENYL_DASSERT(obj.contains("world"));
    PHENYL_DASSERT(obj.contains("test"));
    PHENYL_DASSERT(obj.contains("test2"));

    PHENYL_DASSERT(obj["hello"] == 1);
    PHENYL_DASSERT(obj["world"] == true);

    DataObject obj2 = (DataObject)obj["test"];

    PHENYL_DASSERT(obj2.contains("meme"));
    PHENYL_DASSERT(obj2.contains("testing"));

    //logging::log(LEVEL_DEBUG, "meme = {}", (std::string)obj2Old["meme"]);
    std::string x = (std::string)obj2["meme"];
    PHENYL_DASSERT(x == std::string("lol"));
    PHENYL_DASSERT(obj2["testing"] == 1.1f);

    DataArray arr = (DataArray)obj["test2"];
    PHENYL_DASSERT(arr.size() == 5);

    for (int i = 0; i < arr.size() - 1; i++) {
        PHENYL_DASSERT(arr[i] == arr.size() - i - 1);
    }

    PHENYL_LOGD(LOGGER, "Json: {}", val.convertToJson());
    PHENYL_LOGD(LOGGER, "Json: \n{}", val.convertToJsonPretty());
    PHENYL_DASSERT(val.convertToJson() == parseJson(val.convertToJson()).convertToJson());

}


void phenyl::util::testData () {
    testMap();
    testDataTypes();
    testJson();
}