#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "core/iresource.h"
#include "util/map.h"

#include "axis_action.h"
#include "input_action.h"
#include "input_device.h"

namespace phenyl::core {
    class GameInput : public IResource {
    private:
        util::HashMap<std::string, IInputDevice*> m_devices;
        util::HashMap<std::string, std::unique_ptr<ButtonInputBinding>> m_buttonBindings;
        util::HashMap<std::string, std::unique_ptr<Axis2DBinding>> m_axis2DBindings;

        const ButtonInputSource* getButtonSource (std::string_view sourceId);
        const Axis2DInputSource* getAxis2DSource (std::string_view sourceId);
    public:
        void addDevice (IInputDevice* device);

        InputAction addAction (std::string_view actionId);
        InputAction getAction (std::string_view actionId);
        void addActionBinding (std::string_view actionId, std::string_view sourceId);

        Axis2DInput addAxis2D (std::string_view inputId, bool normalised = false);
        Axis2DInput getAxis2D (std::string_view inputId);
        void addButtonAxis2DBinding (std::string_view inputId, std::string_view sourceId, glm::vec2 buttonAxis);
        void addAxis2DBinding (std::string_view inputId, std::string_view sourceId);

        void update ();

        std::string_view getName () const noexcept override;
    };
}
