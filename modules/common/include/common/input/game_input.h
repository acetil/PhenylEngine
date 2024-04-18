#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "runtime/iresource.h"
#include "util/map.h"

#include "input_action.h"
#include "input_device.h"

namespace phenyl::common {
    class GameInput : public runtime::IResource {
    private:
        util::HashMap<std::string, IInputDevice*> devices;
        util::HashMap<std::string, std::unique_ptr<ButtonInputBinding>> buttonBindings;

        const ButtonInputSource* getSource (std::string_view sourceId);
    public:
        void addDevice (IInputDevice* device);

        InputAction addAction (std::string_view actionId);
        InputAction getAction (std::string_view actionId);
        void addActionBinding (std::string_view actionId, std::string_view sourceId);

        void update ();

        std::string_view getName () const noexcept override;
    };
}
