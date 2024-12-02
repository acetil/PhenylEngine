#pragma once

#include <memory>
#include <vector>

#include "util/map.h"
#include "util/meta.h"

#include "forward.h"

namespace phenyl::core {
    class ResourceManager {
    private:
        std::vector<std::unique_ptr<IResource>> ownedResources;
        util::Map<std::size_t, IResource*> resources;

        void registerResource (std::size_t typeIndex, IResource* resource);
    public:
        template <std::derived_from<IResource> T>
        T& resource () {
            auto* resPtr = resourceMaybe<T>();
            PHENYL_ASSERT_MSG(resPtr, "Attempted to get resource that does not exist!");

            return *resPtr;
        }

        template <std::derived_from<IResource> T>
        const T& resource () const {
            const auto* resPtr = resourceMaybe<T>();
            PHENYL_ASSERT_MSG(resPtr, "Attempted to get resource that does not exist!");

            return *resPtr;
        }

        template <std::derived_from<IResource> T>
        T* resourceMaybe () {
            auto typeIndex = meta::type_index<T>();
            return resources.contains(typeIndex) ? static_cast<T*>(resources[typeIndex]) : nullptr;
        }

        template <std::derived_from<IResource> T>
        const T* resourceMaybe () const {
            auto typeIndex = meta::type_index<T>();
            return resources.contains(typeIndex) ? static_cast<const T*>(resources[typeIndex]) : nullptr;
        }

        template <std::derived_from<IResource> T, typename ...Args>
        void addResource (Args&&... args) requires (std::constructible_from<T, Args...>) {
            auto& res = ownedResources.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
            addResource<T>((T*)res.get());
        }

        template <std::derived_from<IResource> T>
        void addResource (T* resource) {
            registerResource(meta::type_index<T>(), resource);
        }
    };
}