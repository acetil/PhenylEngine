#include <stdlib.h>

//#include "graphics/graphics_headers.h"
//#include "graphics/graphics_init.h"

#include "game/gameloop.h"

#include "logging/logging.h"

//#include "util/format.h"

//#include "util/data.h"
#include "util/set.h"

#include "component/component.h"
#include "util/map.h"

struct Foo {
    int a;
};

struct Bar : public Foo {
    float b;
};

struct Bar2 : public Foo {
    std::string s;
};

struct Baz : public Bar {
    char c;
};

struct TestBase {
    virtual int getVal () = 0;
};

struct Test : TestBase {
    int d;

    int getVal() override {
        return d * 10;
    }

    explicit Test (int d) : d{d} {}
};

struct Test1 : TestBase {
    int e;

    int getVal() override {
        return e * 1000;
    }

    explicit Test1 (int e) : e{e} {}
};

struct Test2 {
    float f;
};

static void testCompManager () {
    component::logging::log(LEVEL_DEBUG, "Starting comp manager tests!");
    auto manager = component::ComponentManager{2};

    manager.addComponent<Foo>();
    manager.addComponent<Bar>();
    manager.addComponent<Bar2>();
    manager.addComponent<Baz>();
    manager.addComponent<Test>();
    manager.addComponent<Test1>();
    manager.addComponent<Test2>();
    manager.addComponent<TestBase>();

    manager.addChild<Foo, Bar>();
    manager.addChild<Foo, Bar2>();
    manager.addChild<Bar, Baz>();

    manager.addChild<TestBase, Test>();
    manager.addChild<TestBase, Test1>();

    manager.addRequirement<Test2, Test>();

    auto e1 = manager.create();
    auto e2 = manager.create();
    auto e3 = manager.create(e2.id());
    auto e4 = manager.create(e2.id());
    auto e5 = manager.create(e2.id());

    component::logging::log(LEVEL_DEBUG, "E1: {}", e1.id().value());
    component::logging::log(LEVEL_DEBUG, "E2: {}", e2.id().value());
    component::logging::log(LEVEL_DEBUG, "E3: {}", e3.id().value());
    component::logging::log(LEVEL_DEBUG, "E4: {}", e4.id().value());
    component::logging::log(LEVEL_DEBUG, "E5: {}", e5.id().value());

    e1.insert<Foo>(Foo{1});
    e2.insert<Bar>(Bar{2, 1.1f});
    e3.insert<Baz>(Baz{3, 2.2f, 'f'});
    e4.insert<Bar2>(Bar2{4, "hello"});

    e5.insert<Test2>(Test2{0.9f});

    e1.insert<Test>(Test{5});
    e2.insert<Test>(Test{6});
    e3.insert<Test>(Test{7});
    e5.insert<Test>(Test{8});

    e1.insert<Test2>(Test2{1.9f});
    e2.insert<Test2>(Test2{1.0f});

    e4.insert<Test2>(Test2{1.7f});

    e4.insert<Test1>(Test1{9});

    component::logging::log(LEVEL_DEBUG, "Looping through Foo:");
    manager.each<Foo>([] (component::IterInfo& info, Foo& foo) {
        component::logging::log(LEVEL_DEBUG, "{} Foo: a={}", info.id().value(), foo.a);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Bar:");
    manager.each<Bar>([] (component::IterInfo& info, Bar& bar) {
        component::logging::log(LEVEL_DEBUG, "{} Bar: a={}, b={}", info.id().value(), bar.a, bar.b);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Bar2:");
    manager.each<Bar2>([] (component::IterInfo& info, Bar2& bar2) {
        component::logging::log(LEVEL_DEBUG, "{} Bar2: a={}, s=\"{}\"", info.id().value(), bar2.a, bar2.s);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Baz:");
    manager.each<Baz>([] (component::IterInfo& info, Baz& baz) {
        component::logging::log(LEVEL_DEBUG, "{} Baz: a={}, b={}, c=\'{}\'", info.id().value(), baz.a, baz.b, baz.c);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Test:");
    manager.each<Test>([] (component::IterInfo& info, Test& test) {
        component::logging::log(LEVEL_DEBUG, "{} Test: d={}", info.id().value(), test.d);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through TestBase:");
    manager.each<TestBase>([] (component::IterInfo& info, TestBase& test) {
        component::logging::log(LEVEL_DEBUG, "{} TestBase: getVal()={}", info.id().value(), test.getVal());
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Test2:");
    manager.each<Test2>([] (component::IterInfo& info, Test2& test) {
        component::logging::log(LEVEL_DEBUG, "{} Test2: f={}", info.id().value(), test.f);
    });

    e5.erase<Test>();

    component::logging::log(LEVEL_DEBUG, "Looping through Test2 after erase:");
    manager.each<Test2>([] (component::IterInfo& info, Test2& test) {
        component::logging::log(LEVEL_DEBUG, "{} Test2: f={}", info.id().value(), test.f);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Foo, Test:");
    manager.each<Foo, Test>([] (component::IterInfo& info, Foo& foo, Test& test) {
        component::logging::log(LEVEL_DEBUG, "{} Foo: a={}, Test: d={}", info.id().value(), foo.a, test.d);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Bar, Test:");
    manager.each<Baz, Test>([] (component::IterInfo& info, Bar& baz, Test& test) {
        component::logging::log(LEVEL_DEBUG, "{} Baz: a={}, b={}, Test: d={}", info.id().value(), baz.a, baz.b, test.d);
    });

    e5.insert<Test>(Test{8});

    component::logging::log(LEVEL_DEBUG, "Looping through Test2 after add back:");
    manager.each<Test2>([] (component::IterInfo& info, Test2& test) {
        component::logging::log(LEVEL_DEBUG, "{} Test2: f={}", info.id().value(), test.f);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Foo pairs:");
    manager.eachPair<Foo>([] (component::ComponentManager::Bundle<Foo> e1, component::ComponentManager::Bundle<Foo> e2) {
        auto& [info1, foo1] = e1;
        auto& [info2, foo2] = e2;

        component::logging::log(LEVEL_DEBUG, "{} Foo: a={}; {} Foo: a={}", info1.id().value(), foo1.a, info2.id().value(), foo2.a);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Test pairs:");
    manager.eachPair<Test>([] (component::ComponentManager::Bundle<Test> e1, component::ComponentManager::Bundle<Test> e2) {
        auto& [info1, test1] = e1;
        auto& [info2, test2] = e2;

        component::logging::log(LEVEL_DEBUG, "{} Test: d={}; {} Test: d={}", info1.id().value(), test1.d, info2.id().value(), test2.d);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through root entities:");
    for (auto i : manager.children(component::EntityId{})) {
        component::logging::log(LEVEL_DEBUG, "{}", i.id().value());
    }

    component::logging::log(LEVEL_DEBUG, "Looping through children of entities:");
    for (auto i : manager) {
        component::logging::log(LEVEL_DEBUG, "Children of {}:", i.id().value());
        for (auto j : i.children()) {
            component::logging::log(LEVEL_DEBUG, "{}", j.id().value());
        }
    }

    e4.reparent(e5.id());

    component::logging::log(LEVEL_DEBUG, "Looping through root entities after reparent:");
    for (auto i : manager.children(component::EntityId{})) {
        component::logging::log(LEVEL_DEBUG, "{}", i.id().value());
    }

    component::logging::log(LEVEL_DEBUG, "Looping through children of entities after reparent:");
    for (auto i : manager) {
        component::logging::log(LEVEL_DEBUG, "Children of {}:", i.id().value());
        for (auto j : i.children()) {
            component::logging::log(LEVEL_DEBUG, "{}", j.id().value());
        }
    }


    component::logging::log(LEVEL_DEBUG, "Testing hierarchy insertion:", e1.id().value());
    e1.insert<Bar>(Bar{9, 9.9f});
    e4.insert<Foo>(Foo{10});

    component::logging::log(LEVEL_DEBUG, "Looping through Foo:");
    manager.each<Foo>([] (component::IterInfo& info, Foo& foo) {
        component::logging::log(LEVEL_DEBUG, "{} Foo: a={}", info.id().value(), foo.a);
    });

    component::logging::log(LEVEL_DEBUG, "Looping through Bar:");
    manager.each<Bar>([] (component::IterInfo& info, Bar& bar) {
        component::logging::log(LEVEL_DEBUG, "{} Bar: a={}, b={}", info.id().value(), bar.a, bar.b);
    });

    e2.remove();

    component::logging::log(LEVEL_DEBUG, "Looping through root entities after remove:");
    for (auto i : manager.children(component::EntityId{})) {
        component::logging::log(LEVEL_DEBUG, "{}", i.id().value());
    }

    component::logging::log(LEVEL_DEBUG, "Looping through children of entities after remove:");
    for (auto i : manager) {
        component::logging::log(LEVEL_DEBUG, "Children of {}:", i.id().value());
        for (auto j : i.children()) {
            component::logging::log(LEVEL_DEBUG, "{}", j.id().value());
        }
    }
};

int main (int argv, char* argc[]) {
    // TODO: move to exceptions
    //util::testData();

    logger::initLogger();
    testCompManager();

    logger::log(LEVEL_DEBUG, "MAIN", "Started game!");
    /*logger::log(LEVEL_DEBUG, "MAIN", util::format("Test format: {} {} {1}", 4, 3.8, "abc"));
    GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Window init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    graphics::Graphics::SharedPtr graphics;
    if (graphics::initGraphics(window, graphics) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Graphics init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    logger::log(LEVEL_INFO, "MAIN", "Successfully initialised graphics");
    game::gameloop(graphics);
    logger::log(LEVEL_INFO, "MAIN", "Shutting down!");
    graphics::destroyGraphics(graphics);*/

    engine::PhenylEngine engine;

    game::gameloop(engine);

    return EXIT_SUCCESS;
}