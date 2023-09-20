#pragma once

#include <cstddef>
#include <iosfwd>

#include "forward.h"
#include "util/meta.h"

namespace phenyl::common {
    class Assets;
    namespace detail {
        class AssetManagerBase {
        private:
            virtual void queueUnload (std::size_t id) = 0;
            [[nodiscard]] virtual const char* getFileType () const = 0;
            virtual bool isBinary () const {
                return false;
            }

            static bool OnUnloadUntyped (std::size_t typeIndex, std::size_t id);

            friend class common::Assets;
            template <typename T>
            friend class common::AssetManager;
        public:
            virtual ~AssetManagerBase () = default;
        };
    }

    template <typename T>
    class AssetManager : public detail::AssetManagerBase {
    private:
        friend class Assets;
    protected:
        virtual T* load (std::istream& data, std::size_t id) = 0;
        bool onUnload (std::size_t id) {
            return detail::AssetManagerBase::OnUnloadUntyped(meta::type_index<T>(), id);
        }
    };
}