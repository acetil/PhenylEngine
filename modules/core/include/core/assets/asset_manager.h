#pragma once

#include "forward.h"
#include "load_context.h"
#include "util/type_index.h"

#include <cstddef>
#include <iosfwd>
#include <memory>

namespace phenyl::core {
class Assets;

namespace detail {
    class AssetManagerBase {
    public:
        virtual ~AssetManagerBase () = default;

    private:
        friend class core::Assets;
        template <typename T>
        friend class core::AssetManager;
    };
} // namespace detail

template <typename T>
class AssetManager : public detail::AssetManagerBase {
public:
    virtual std::shared_ptr<T> load (AssetLoadContext& ctx) = 0;

    virtual void onVirtualLoad (std::shared_ptr<T> obj) {}

private:
    friend class Assets;
};
} // namespace phenyl::core
