#pragma once

#include "forward.h"
#include "load_context.h"

#include <memory>

namespace phenyl::core {
template <typename T>
class AssetManager {
public:
    virtual ~AssetManager () = default;

    virtual std::shared_ptr<T> load (AssetLoadContext& ctx) = 0;

    virtual void onVirtualLoad (const std::shared_ptr<T>& obj) {}

private:
    friend class Assets;
};
} // namespace phenyl::core
