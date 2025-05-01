#include <dlfcn.h>
#include <iostream>

#include "platform/dynamic_library.h"

#include "logging/logging.h"

using namespace phenyl::os;

namespace phenyl::os::detail {
    class DynamicLibImpl {
    public:
        void* handle;

        explicit DynamicLibImpl (void* handle) : handle{handle} {
            PHENYL_DASSERT(handle);
        }

        ~DynamicLibImpl() {
            dlclose(handle);
        }
    };
}

phenyl::Logger LOGGER{"DYNAMIC_LIBRARY", phenyl::PHENYL_LOGGER};

DynamicLibrary::DynamicLibrary (const std::string& path) {
    if (auto* handle = dlopen(path.c_str(), RTLD_NOW)) {
        PHENYL_LOGD(LOGGER, "Successfully loaded dynamic library \"{}\"", path);
        m_impl = std::make_shared<detail::DynamicLibImpl>(handle);
        //std::cerr << std::format("Successfully loaded dynamic library \"{}\"\n", path);
    } else {
        PHENYL_LOGE(LOGGER, "Failed to open dynamic library \"{}\" with error: {}", path, dlerror());
        //std::cerr << std::format("Failed to open dynamic library \"{}\" with error: {}\n", path, dlerror());
    }
}

DynamicLibrary::DynamicLibrary (DynamicLibrary&& other) noexcept = default;
DynamicLibrary& DynamicLibrary::operator= (DynamicLibrary&& other) noexcept = default;

DynamicLibrary::~DynamicLibrary () = default;

void* DynamicLibrary::loadFunctionSymbol (const std::string& symbolName) const {
    if (!m_impl) {
        PHENYL_LOGE(LOGGER, "Attempted to load symbol \"{}\" from invalid library!", symbolName);
        //std::cerr << std::format("Attempted to load symbol {} from invalid library!\n", symbolName);
        return nullptr;
    }

    if (auto* symbol = dlsym(m_impl->handle, symbolName.c_str())) {
        PHENYL_LOGD(LOGGER, "Successfully loaded symbol \"{}\"", symbolName);
        return symbol;
    } else {
        PHENYL_LOGE(LOGGER, "Failed to load function symbol \"{}\" with error: {}", symbolName, dlerror());
        std::cerr << std::format("Failed to load function symbol \"{}\" with error: {}\n", symbolName, dlerror());
        return nullptr;
    }
}

