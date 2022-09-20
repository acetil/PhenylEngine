#pragma once

#include <memory>
#include <vector>

#include "common/input/remappable_input.h"
#include "event/event_bus.h"

namespace game {
    class EmptyEventAction {
        int a;
    };
    namespace detail {

        template <typename T>
        class EventActionRaiser {
        private:
            T event;
        public:
            EventActionRaiser (T _event) : event{_event} {};
            void raiseEvent (const event::EventBus::SharedPtr& eventBus) {
                eventBus->raise(event);
            }
        };

        template <>
        class EventActionRaiser<EmptyEventAction> {
        public:
            EventActionRaiser (EmptyEventAction action) {};
            void raiseEvent (const event::EventBus::SharedPtr& eventBus) {

            }
        };

        class EventAction {
        private:
            common::InputAction action;
            bool isDown = false;
        protected:
            virtual void onPress (const event::EventBus::SharedPtr& eventBus) = 0;
            virtual void onRelease (const event::EventBus::SharedPtr& eventBus) = 0;
        public:
            EventAction (common::InputAction _action) : action{_action} {}
            virtual ~EventAction() = default;
            void poll (std::unique_ptr<common::RemappableInput>& inputSource, const event::EventBus::SharedPtr& eventBus) {
                bool curr = inputSource->isActive(action);

                if (curr != isDown) {
                    if (curr) {
                        onPress(eventBus);
                    } else {
                        onRelease(eventBus);
                    }
                    isDown = curr;
                }
            }
        };


        template<typename T, typename U>
        class EventActionImpl : public EventAction {
            detail::EventActionRaiser<T> pressEventRaiser;
            detail::EventActionRaiser<U> releaseEventRaiser;

        protected:
            void onPress(const event::EventBus::SharedPtr& eventBus) override {
                pressEventRaiser.raiseEvent(eventBus);
            }

            void onRelease(const event::EventBus::SharedPtr& eventBus) override {
                releaseEventRaiser.raiseEvent(eventBus);
            }

        public:
            EventActionImpl (common::InputAction _action, T _pressEvent, U _releaseEvent) : EventAction(_action), pressEventRaiser{_pressEvent}, releaseEventRaiser{_releaseEvent} {};
        };
    }
    class GameInput {
    private:
        std::unique_ptr<common::RemappableInput> inputSource;
        event::EventBus::SharedPtr eventBus;
        std::vector<std::unique_ptr<detail::EventAction>> eventActions;
    public:
        GameInput ();
        void setEventBus (event::EventBus::SharedPtr eventBus);
        common::InputAction mapInput (const std::string& actionName, const std::string& inputName);
        common::InputAction getInput (const std::string& actionName);

        bool isDown (const common::InputAction& action);

        void poll ();

        template <typename T, typename U>
        void addInputEvent (common::InputAction action, T pressEvent, U releaseEvent) {
            eventActions.emplace_back(std::make_unique<detail::EventActionImpl<T, U>>(action, pressEvent, releaseEvent));
        }

        template <typename T>
        void addInputEvent (common::InputAction action, T pressEvent) {
            eventActions.emplace_back(std::make_unique<detail::EventActionImpl<T, EmptyEventAction>>(action, pressEvent, EmptyEventAction{}));
        }

        void addInputSources (const std::vector<std::shared_ptr<common::InputSource>>& sources);
    };
}