#pragma once

#include "component.h"
#include "node.h"

namespace phenyl::graphics {
class UIRoot;

class UI : public core::IResource {
public:
    virtual std::size_t makeId () = 0;
    virtual void onComponentDestroy (std::size_t id) = 0;

    virtual void markDirty (std::size_t id) = 0;

    virtual UIRoot& root () = 0;

    std::string_view getName () const noexcept override;
};
} // namespace phenyl::graphics
