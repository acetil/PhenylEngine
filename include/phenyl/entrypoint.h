#pragma once

#include "engine.h"
#include "properties.h"

extern "C" {
    void phenyl_app_entrypoint (void* enginePtr, void* propertiesPtr);
}

#ifndef PHENYL_APP_LIBRARY
#define PHENYL_MAIN() int main (int argc, char* argv[]) { \
    ::phenyl::PhenylEngine engine; \
    ::phenyl::ApplicationProperties properties{}; \
    phenyl_app_entrypoint(&engine, &properties); \
    return 0; \
}
#else
#define PHENYL_MAIN()
#endif

#define PHENYL_ENTRYPOINT(ApplicationType) extern "C" {\
    void phenyl_app_entrypoint (void* enginePtr, void* propertiesPtr) {\
        auto* engine = reinterpret_cast<::phenyl::PhenylEngine*>(enginePtr);\
        auto* properties = reinterpret_cast<::phenyl::ApplicationProperties*>(propertiesPtr);\
        engine->run<ApplicationType>(std::move(*properties)); \
    }\
}
//PHENYL_MAIN()
