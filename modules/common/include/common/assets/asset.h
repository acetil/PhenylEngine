#pragma once

#include <cassert>
#include <cstddef>
#include <string>

#include "logging/logging.h"
#include "util/meta.h"
#include "forward.h"
#include "common/serialization/serializer.h"

namespace phenyl::common {
    namespace detail {
        template <typename T>
        class AssetCache;

        class AssetBase {
        private:
            static void IncRefCount (std::size_t typeIndex, std::size_t id);
            static void DecRefCount (std::size_t typeIndex, std::size_t id);

            static std::string_view GetPath (std::size_t typeIndex, std::size_t id);

            template <typename T>
            friend class common::Asset;
        };
    }
    template <typename T>
    class Asset {
    private:
        std::size_t rId;
        T* ptr;

        friend class Assets;
        friend class detail::AssetCache<T>;
        Asset (std::size_t id, T* ptr) : rId{id}, ptr{ptr} {}
    public:
        Asset () : rId{0}, ptr{nullptr} {}

        Asset (const Asset<T>& other) : rId{other.rId}, ptr{other.ptr} {
            detail::AssetBase::IncRefCount(meta::type_index<T>(), rId);
        }
        Asset (Asset<T>&& other) noexcept : rId{other.rId}, ptr{other.ptr} {
            other.rId = 0;
            other.ptr = nullptr;
        }

        Asset<T>& operator= (const Asset<T>& other) {
            if (&other == this) {
                return *this;
            }

            if (rId) {
                detail::AssetBase::DecRefCount(meta::type_index<T>(), rId);
            }

            rId = other.rId;
            ptr = other.ptr;
            detail::AssetBase::IncRefCount(meta::type_index<T>(), rId);
            return *this;
        }
        Asset<T>& operator= (Asset<T>&& other) noexcept {
            if (rId) {
                detail::AssetBase::DecRefCount(meta::type_index<T>(), rId);
            }

            rId = other.rId;
            ptr = other.ptr;
            other.rId = 0;
            other.ptr = nullptr;
            return *this;
        }

        ~Asset () noexcept {
            if (rId) {
                detail::AssetBase::DecRefCount(meta::type_index<T>(), rId);
            }
        }

        explicit operator bool () const {
            return rId && ptr;
        }

        T& operator* () {
            PHENYL_DASSERT(ptr);
            return *ptr;
        }

        const T& operator* () const {
            PHENYL_DASSERT(ptr);
            return *ptr;
        }

        /*T* operator-> () {
            return ptr;
        }*/

        T* operator-> () const {
            return ptr;
        }

        T* get () {
            return ptr;
        }

        const T* get () const {
            return ptr;
        }

        [[nodiscard]] std::size_t id () const {
            return rId;
        }

        [[nodiscard]] std::string_view path () const {
            return detail::AssetBase::GetPath(meta::type_index<T>(), rId);
        }
    };

    template <typename T>
    ISerializable<Asset<T>>& phenyl_GetSerializable (detail::SerializableMarker<Asset<T>>);
}