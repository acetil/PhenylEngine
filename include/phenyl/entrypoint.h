#pragma once

#include "engine.h"
#include "properties.h"

extern "C" {
    void phenyl_app_entrypoint (void* propertiesPtr, void* appOut);
}

#define PHENYL_ENTRYPOINT(ApplicationType) extern "C" {\
    void phenyl_app_entrypoint (void* propertiesPtr, void* appOutPtr) { \
        auto* properties = reinterpret_cast<::phenyl::ApplicationProperties*>(propertiesPtr); \
        auto* appOut = reinterpret_cast<std::unique_ptr<::phenyl::engine::ApplicationBase>*>(appOutPtr); \
        *appOut = std::make_unique<ApplicationType>(std::move(*properties)); \
    } \
}
