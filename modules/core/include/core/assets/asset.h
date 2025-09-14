#pragma once

#include "core/serialization/serializer.h"
#include "forward.h"
#include "logging/logging.h"
#include "util/type_index.h"

#include <cassert>
#include <cstddef>
#include <string>

namespace phenyl::core {
namespace detail {
    template <typename T>
    class AssetCache;
} // namespace detail

class AssetBase {
public:
    virtual ~AssetBase ();

    AssetBase (const AssetBase& other);
    AssetBase (AssetBase&& other) noexcept;

    AssetBase& operator= (const AssetBase& other);
    AssetBase& operator= (AssetBase&& other) noexcept;

    [[nodiscard]] std::size_t assetId () const noexcept {
        return m_id;
    }

protected:
    explicit AssetBase (meta::TypeIndex type);

private:
    std::size_t m_id;
    meta::TypeIndex m_type;

    friend class Assets;
    template <typename T>
    friend class detail::AssetCache;
};

template <typename T> concept NamedAssetType = requires {
    { T::GetAssetType() } -> std::same_as<std::string_view>;
};

template <typename T>
class Asset : public AssetBase {
public:
    static std::string_view Name () {
        return T::GetAssetType();
    }

    Asset () : AssetBase{meta::TypeIndex::Get<T>()} {
        static_assert(NamedAssetType<T>);
    }
};

template <typename T> concept AssetType = NamedAssetType<T> && std::derived_from<T, Asset<T>>;

template <AssetType T>
ISerializable<std::shared_ptr<T>>& phenyl_GetSerializable(detail::SerializableMarker<std::shared_ptr<T>>);
} // namespace phenyl::core
