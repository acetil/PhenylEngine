#pragma once

#include "forward.h"
#include "util/type_index.h"

#include <cstddef>
#include <iosfwd>

namespace phenyl::core {
class Assets;

namespace detail {
    class AssetManagerBase {
    public:
        virtual ~AssetManagerBase () = default;

    private:
        virtual void queueUnload (std::size_t id) = 0;
        [[nodiscard]] virtual const char* getFileType () const = 0;

        virtual bool isBinary () const {
            return false;
        }

        static bool OnUnloadUntyped (meta::TypeIndex typeIndex, std::size_t id);

        static std::size_t onVirtualLoadUntyped (meta::TypeIndex typeIndex, const std::string& virtualPath,
            std::byte* data);

        friend class core::Assets;
        template <typename T>
        friend class core::AssetManager;
    };
} // namespace detail

template <typename T>
class AssetManager : public detail::AssetManagerBase {
protected:
    virtual T* load (std::ifstream& data, std::size_t id) = 0;
    virtual T* load (T&& obj, std::size_t id) = 0;

    bool onUnload (std::size_t id) {
        return detail::AssetManagerBase::OnUnloadUntyped(meta::TypeIndex::Get<T>(), id);
    }

    std::size_t onVirtualLoad (const std::string& virtualPath, T* data) {
        return onVirtualLoadUntyped(meta::TypeIndex::Get<T>(), virtualPath, (std::byte*) data);
    }

private:
    friend class Assets;
};
} // namespace phenyl::core
