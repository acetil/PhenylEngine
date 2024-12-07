#include <iostream>

#include <phenyl/entrypoint.h>
#include <phenyl/platform.h>

int main (int argc, char* argv[]) {
    // TODO: parse cmdline args
    //phenyl_app_entrypoint(&engine, &properties);
    phenyl::ApplicationProperties properties;
    auto props = phenyl::ApplicationProperties{}
        .withLogFile("debug.log")
        .withRootLogLevel(LEVEL_DEBUG);
    phenyl::PhenylEngine engine{props.logging()};

    phenyl::os::DynamicLibrary library{PHENYL_APP_LIB};
    if (!library) {
        std::cerr << "Failed to load application library \"" << PHENYL_APP_LIB << "\"\n";
        return EXIT_FAILURE;
    }

    auto entrypoint = library.function<void, void*, void*>("phenyl_app_entrypoint");
    if (!entrypoint) {
        std::cerr << "Failed to load application entrypoint\n";
        return EXIT_FAILURE;
    }
    std::unique_ptr<phenyl::engine::ApplicationBase> app;
    (*entrypoint)(&props, &app);

    PHENYL_ASSERT(app);
    engine.run(std::move(app));
}