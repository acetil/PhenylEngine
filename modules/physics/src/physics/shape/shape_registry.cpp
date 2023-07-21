#include "physics/2d/shape_registry_2d.h"

/*struct Test {
    int x;

    Test (int a) : x{a} {}

    Test (const Test&) = delete;
    Test (Test&&) = default;
    Test& operator= (const Test&) = delete;
    Test& operator= (Test&&) = default;

    ~Test() = default;
};

struct TestTag{};

void physics::test_fixed () {
    component::FixedComponentManager<4, int, float, Test> manager{};
    using Id = component::FixedComponentManager<4, int, float, Test>::IdType;
    Id id1 = manager.addComponent<int>(3);
    Id id2 = manager.addComponent<int>(4);
    Id id3 = manager.insert<int>(5);

    logging::log(LEVEL_DEBUG, "Fixed ids: {}, {}, {}", id1.getValue(), id2.getValue(), id3.getValue());

    for (std::pair<int&, Id> i : manager.iterate<int>()) {
        physics::logging::log(LEVEL_DEBUG, "Id: {}, val: {}", i.second.getValue(), i.first);

        i.first *= 2;
    }

    for (auto [i, entityId] : manager.iterate<int>()) {
        physics::logging::log(LEVEL_DEBUG, "2nd: Id: {}, val: {}", entityId.getValue(), i);
    }

    manager.remove(id1);
    auto id4 = manager.insert<int>(12);
    logging::log(LEVEL_DEBUG, "Fixed entityId 4: {}", id4.getValue());

    for (auto [i, entityId] : manager.iterate<int>()) {
        physics::logging::log(LEVEL_DEBUG, "3rd: Id: {}, val: {}", entityId.getValue(), i);
    }
}*/