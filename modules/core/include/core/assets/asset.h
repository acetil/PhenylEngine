#pragma once

#include <cassert>
#include <cstddef>
#include <string>

#include "logging/logging.h"
#include "util/meta.h"
#include "forward.h"
#include "core/serialization/serializer.h"

namespace phenyl::core {
    template <typename T>
    class IAssetType;

    namespace detail {
        template <typename T>
        class AssetCache;

        class AssetBase {
        private:
            static void IncRefCount (std::size_t typeIndex, std::size_t id);
            static void DecRefCount (std::size_t typeIndex, std::size_t id);

            static std::string_view GetPath (std::size_t typeIndex, std::size_t id);

            template <typename T>
            friend class core::Asset;

            template <typename T>
            friend class core::IAssetType;
        };
    }

    template <typename T>
    class Asset {
    public:
        Asset () : m_id{0}, m_ptr{nullptr} {}

        Asset (const Asset<T>& other) : m_id{other.m_id}, m_ptr{other.m_ptr} {
            detail::AssetBase::IncRefCount(meta::type_index<T>(), m_id);
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
                detail::AssetBase::DecRefCount(meta::type_index<T>(), m_id);
            }

            m_id = other.m_id;
            m_ptr = other.m_ptr;
            detail::AssetBase::IncRefCount(meta::type_index<T>(), m_id);
            return *this;
        }
        Asset<T>& operator= (Asset<T>&& other) noexcept {
            if (m_id) {
                detail::AssetBase::DecRefCount(meta::type_index<T>(), m_id);
            }

            m_id = other.m_id;
            m_ptr = other.m_ptr;
            other.m_id = 0;
            other.m_ptr = nullptr;
            return *this;
        }

        ~Asset () noexcept {
            if (m_id) {
                detail::AssetBase::DecRefCount(meta::type_index<T>(), m_id);
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

        T* operator-> () const {
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
            return detail::AssetBase::GetPath(meta::type_index<T>(), m_id);
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

            detail::AssetBase::IncRefCount(meta::type_index<T>(), *m_id);

            return Asset<T>{*m_id, static_cast<T*>(this)};
        }

        friend class Assets;

    private:
        std::optional<std::size_t> m_id = 0;
    };

    template <typename T>
    ISerializable<Asset<T>>& phenyl_GetSerializable (detail::SerializableMarker<Asset<T>>);
}