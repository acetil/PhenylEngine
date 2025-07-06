#pragma once

#include "core/serialization/serializer.h"
#include "forward.h"
#include "logging/logging.h"
#include "util/type_index.h"

#include <cassert>
#include <cstddef>
#include <string>

namespace phenyl::core {
template <typename T>
class IAssetType;

namespace detail {
    template <typename T>
    class AssetCache;

    template <typename T>
    class AssetCache2;

    class AssetBase {
    private:
        static void IncRefCount (meta::TypeIndex typeIndex, std::size_t id);
        static void DecRefCount (meta::TypeIndex typeIndex, std::size_t id);

        static std::string_view GetPath (meta::TypeIndex typeIndex, std::size_t id);

        template <typename T>
        friend class core::Asset;

        template <typename T>
        friend class core::IAssetType;
    };
} // namespace detail

template <typename T>
class Asset {
public:
    Asset () : m_id{0}, m_ptr{nullptr} {}

    Asset (const Asset<T>& other) : m_id{other.m_id}, m_ptr{other.m_ptr} {
        detail::AssetBase::IncRefCount(meta::TypeIndex::Get<T>(), m_id);
    }

    Asset (Asset<T>&& other) noexcept : m_id{other.m_id}, m_ptr{other.m_ptr} {
        other.m_id = 0;
        other.m_ptr = nullptr;
    }

    Asset<T>& operator= (const Asset<T>& other) {
        if (&other == this) {
            return *this;
        }

        if (m_id) {
            detail::AssetBase::DecRefCount(meta::TypeIndex::Get<T>(), m_id);
        }

        m_id = other.m_id;
        m_ptr = other.m_ptr;
        detail::AssetBase::IncRefCount(meta::TypeIndex::Get<T>(), m_id);
        return *this;
    }

    Asset<T>& operator= (Asset<T>&& other) noexcept {
        if (m_id) {
            detail::AssetBase::DecRefCount(meta::TypeIndex::Get<T>(), m_id);
        }

        m_id = other.m_id;
        m_ptr = other.m_ptr;
        other.m_id = 0;
        other.m_ptr = nullptr;
        return *this;
    }

    ~Asset () noexcept {
        if (m_id) {
            detail::AssetBase::DecRefCount(meta::TypeIndex::Get<T>(), m_id);
        }
    }

    explicit operator bool () const {
        return m_id && m_ptr;
    }

    T& operator* () {
        PHENYL_DASSERT(m_ptr);
        return *m_ptr;
    }

    const T& operator* () const {
        PHENYL_DASSERT(m_ptr);
        return *m_ptr;
    }

    T* operator->() const {
        return m_ptr;
    }

    T* get () {
        return m_ptr;
    }

    const T* get () const {
        return m_ptr;
    }

    [[nodiscard]] std::size_t id () const {
        return m_id;
    }

    [[nodiscard]] std::string_view path () const {
        return detail::AssetBase::GetPath(meta::TypeIndex::Get<T>(), m_id);
    }

private:
    std::size_t m_id;
    T* m_ptr;

    friend class Assets;
    friend class IAssetType<T>;
    friend class detail::AssetCache<T>;

    Asset (std::size_t id, T* ptr) : m_id{id}, m_ptr{ptr} {}
};

template <typename T>
class IAssetType {
public:
    virtual ~IAssetType () = default;

    Asset<T> assetFromThis () {
        if (!m_id) {
            return Asset<T>{};
        }

        detail::AssetBase::IncRefCount(meta::TypeIndex::Get<T>(), *m_id);

        return Asset<T>{*m_id, static_cast<T*>(this)};
    }

    friend class Assets;

private:
    std::optional<std::size_t> m_id = 0;
};

class AssetTypeUntyped {
public:
    AssetTypeUntyped (meta::TypeIndex type);
    AssetTypeUntyped (const AssetTypeUntyped& other);
    AssetTypeUntyped (AssetTypeUntyped&& other) noexcept;

    AssetTypeUntyped& operator= (const AssetTypeUntyped& other);
    AssetTypeUntyped& operator= (AssetTypeUntyped&& other) noexcept;
    virtual ~AssetTypeUntyped ();

    std::size_t assetId () const noexcept {
        return m_id;
    }

private:
    std::size_t m_id;
    meta::TypeIndex m_type;

    friend class Assets;
    template <typename T>
    friend class detail::AssetCache2;
};

template <typename T>
class AssetType2 : public AssetTypeUntyped {
public:
    AssetType2 () : AssetTypeUntyped{meta::TypeIndex::Get<T>()} {}
};

template <typename T> concept IsAssetType = std::derived_from<T, AssetType2<T>>;

template <typename T>
ISerializable<Asset<T>>& phenyl_GetSerializable(detail::SerializableMarker<Asset<T>>);

template <IsAssetType T>
ISerializable<std::shared_ptr<T>> phenyl_GetSerializable(detail::SerializableMarker<std::shared_ptr<T>>);
} // namespace phenyl::core
