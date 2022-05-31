#pragma once

#include <vector>

#include "input_source.h"

#include "util/map.h"

namespace common {

    namespace detail {
        struct SourceAction {
            size_t sourceIndex{0};
            long actionIndex{-1};

            operator bool () const {
                return actionIndex >= 0;
            }
        };
    }

    class RemappableInput;

    class InputAction {
    private:
        uint32_t actionIndex{};
        explicit InputAction (uint32_t _actionIndex) : actionIndex{_actionIndex} {}
    public:
        InputAction() = default;

        operator bool () const {
            return actionIndex != 0;
        }
        friend class RemappableInput;
    };

    class RemappableInput {
    protected:
        std::vector<std::shared_ptr<InputSource>> inputSources;
        util::Map<std::string, InputAction> actionMap;
        std::vector<detail::SourceAction> actions;

        detail::SourceAction getSourceAction (const std::string& inputName);
        uint32_t getActionIndex (InputAction action);

    public:
        void addInputSource (const std::shared_ptr<InputSource>& source);
        InputAction addInputMapping (const std::string& actionName, const std::string& inputName);
        InputAction getInputAction (const std::string& actionName);
        void setInputMapping (const InputAction& action, const std::string& inputName);

        bool isActive (const InputAction& action);
    };
}