#include <stdlib.h>

#include <logging/logging.h>
#include <phenyl/phenyl.h>

#include "game/test_app.h"

using namespace phenyl;

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

static phenyl::Logger LOGGER{"MAIN"};

static void testCompManager () {
    InitLogging(logging::LoggingProperties{}.withRootLogLevel(LEVEL_DEBUG).withLogFile("comp_manager.log"));
    PHENYL_LOGD(LOGGER, "Starting comp manager tests!");
    auto manager = phenyl::ComponentManager{2};

    manager.addComponent<Foo>();
    manager.addComponent<Bar>();
    manager.addComponent<Bar2>();
    manager.addComponent<Baz>();
    manager.addComponent<Test>();
    manager.addComponent<Test1>();
    manager.addComponent<Test2>();
    manager.addComponent<TestBase>();

    manager.inherits<Bar, Foo>();
    manager.inherits<Bar2, Foo>();
    manager.inherits<Baz, Bar>();

    manager.inherits<Test, TestBase>();
    manager.inherits<Test1, TestBase>();

    manager.addRequirement<Test2, Test>();

    auto e1 = manager.create();
    auto e2 = manager.create();
    auto e3 = e2.createChild();
    auto e4 = e2.createChild();
    auto e5 = e2.createChild();

    PHENYL_LOGD(LOGGER, "E1: {}", e1.id().value());
    PHENYL_LOGD(LOGGER, "E2: {}", e2.id().value());
    PHENYL_LOGD(LOGGER, "E3: {}", e3.id().value());
    PHENYL_LOGD(LOGGER, "E4: {}", e4.id().value());
    PHENYL_LOGD(LOGGER, "E5: {}", e5.id().value());

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

    PHENYL_LOGD(LOGGER, "Looping through Foo:");
    manager.query<Foo>().each([] (component::Entity entity, Foo& foo) {
        PHENYL_LOGD(LOGGER, "{} Foo: a={}", entity.id().value(), foo.a);
    });

    PHENYL_LOGD(LOGGER, "Looping through Bar:");
    manager.query<Bar>().each([] (component::Entity entity, Bar& bar) {
        PHENYL_LOGD(LOGGER, "{} Bar: a={}, b={}", entity.id().value(), bar.a, bar.b);
    });

    PHENYL_LOGD(LOGGER, "Looping through Bar2:");
    manager.query<Bar2>().each([] (component::Entity entity, Bar2& bar2) {
        PHENYL_LOGD(LOGGER, "{} Bar2: a={}, s=\"{}\"", entity.id().value(), bar2.a, bar2.s);
    });

    PHENYL_LOGD(LOGGER, "Looping through Baz:");
    manager.query<Baz>().each([] (component::Entity entity, Baz& baz) {
        PHENYL_LOGD(LOGGER, "{} Baz: a={}, b={}, c=\'{}\'", entity.id().value(), baz.a, baz.b, baz.c);
    });

    PHENYL_LOGD(LOGGER, "Looping through Test:");
    manager.query<Test>().each([] (component::Entity entity, Test& test) {
        PHENYL_LOGD(LOGGER, "{} Test: d={}", entity.id().value(), test.d);
    });

    PHENYL_LOGD(LOGGER, "Looping through TestBase:");
    manager.query<TestBase>().each([] (component::Entity entity, TestBase& test) {
        PHENYL_LOGD(LOGGER, "{} TestBase: getVal()={}", entity.id().value(), test.getVal());
    });

    PHENYL_LOGD(LOGGER, "Looping through Test2:");
    manager.query<Test2>().each([] (component::Entity entity, Test2& test) {
        PHENYL_LOGD(LOGGER, "{} Test2: f={}", entity.id().value(), test.f);
    });

    e5.erase<Test>();

    PHENYL_LOGD(LOGGER, "Looping through Test2 after erase:");
    manager.query<Test2>().each([] (component::Entity entity, Test2& test) {
        PHENYL_LOGD(LOGGER, "{} Test2: f={}", entity.id().value(), test.f);
    });

    e1.set(Foo{19});

    PHENYL_LOGD(LOGGER, "Looping through Foo, Test:");
    manager.query<Foo, Test>().each([] (component::Entity entity, Foo& foo, Test& test) {
        PHENYL_LOGD(LOGGER, "{} Foo: a={}, Test: d={}", entity.id().value(), foo.a, test.d);
    });

    PHENYL_LOGD(LOGGER, "Looping through Bar, Test:");
    manager.query<Baz, Test>().each([] (component::Entity entity, Bar& baz, Test& test) {
        PHENYL_LOGD(LOGGER, "{} Baz: a={}, b={}, Test: d={}", entity.id().value(), baz.a, baz.b, test.d);
    });

    e5.set(Test{8});

    PHENYL_LOGD(LOGGER, "Looping through Test2 after add back:");
    manager.query<Test2>().each([] (component::Entity entity, Test2& test) {
        PHENYL_LOGD(LOGGER, "{} Test2: f={}", entity.id().value(), test.f);
    });

    PHENYL_LOGD(LOGGER, "Looping through Foo pairs:");
    manager.query<Foo>().pairs([] (component::QueryBundle<Foo> e1, component::QueryBundle<Foo> e2) {
        //auto& [info1, foo1] = e1;

        //auto& [info2, foo2] = e2;

        PHENYL_LOGD(LOGGER, "{} Foo: a={}; {} Foo: a={}", e1.entity().id().value(), e1.get<Foo>().a, e2.entity().id().value(), e2.get<Foo>().a);
    });

    PHENYL_LOGD(LOGGER, "Looping through Test pairs:");
    manager.query<Test>().pairs([] (component::QueryBundle<Test> e1, component::QueryBundle<Test> e2) {
        //auto& [info1, test1] = e1;
        //auto& [info2, test2] = e2;

        PHENYL_LOGD(LOGGER, "{} Test: d={}; {} Test: d={}", e1.entity().id().value(), e1.get<Test>().d, e2.entity().id().value(), e2.get<Test>().d);
    });

    PHENYL_LOGD(LOGGER, "Looping through root entities:");
    for (auto i : manager.root()) {
        PHENYL_LOGD(LOGGER, "{}", i.id().value());
    }

    PHENYL_LOGD(LOGGER, "Looping through children of entities:");
    for (auto i : manager) {
        PHENYL_LOGD(LOGGER, "Children of {}:", i.id().value());
        for (auto j : i.children()) {
            PHENYL_LOGD(LOGGER, "{}", j.id().value());
        }
    }

    //e4.reparent(e5.id());
    e5.addChild(e4);

    PHENYL_LOGD(LOGGER, "Looping through root entities after reparent:");
    for (auto i : manager.root()) {
        PHENYL_LOGD(LOGGER, "{}", i.id().value());
    }

    PHENYL_LOGD(LOGGER, "Looping through children of entities after reparent:");
    for (auto i : manager) {
        PHENYL_LOGD(LOGGER, "Children of {}:", i.id().value());
        for (auto j : i.children()) {
            PHENYL_LOGD(LOGGER, "{}", j.id().value());
        }
    }


    PHENYL_LOGD(LOGGER, "Testing hierarchy insertion:", e1.id().value());
    e1.insert<Bar>(Bar{9, 9.9f});
    e4.insert<Foo>(Foo{10});

    PHENYL_LOGD(LOGGER, "Looping through Foo:");
    manager.query<Foo>().each([] (component::Entity entity, Foo& foo) {
        PHENYL_LOGD(LOGGER, "{} Foo: a={}", entity.id().value(), foo.a);
    });

    PHENYL_LOGD(LOGGER, "Looping through Bar:");
    manager.query<Bar>().each([] (component::Entity entity, Bar& bar) {
        PHENYL_LOGD(LOGGER, "{} Bar: a={}, b={}", entity.id().value(), bar.a, bar.b);
    });

    e2.remove();

    PHENYL_LOGD(LOGGER, "Looping through root entities after remove:");
    for (auto i : manager.root()) {
        PHENYL_LOGD(LOGGER, "{}", i.id().value());
    }

    PHENYL_LOGD(LOGGER, "Looping through children of entities after remove:");
    for (auto i : manager) {
        PHENYL_LOGD(LOGGER, "Children of {}:", i.id().value());
        for (auto j : i.children()) {
            PHENYL_LOGD(LOGGER, "{}", j.id().value());
        }
    }

    ShutdownLogging();
};

int main (int argv, char* argc[]) {
    phenyl::PhenylEngine engine;
    //testCompManager();

    PHENYL_LOGD(LOGGER, "Started game!");
    engine.run<test::TestApp>();

    return EXIT_SUCCESS;
}
