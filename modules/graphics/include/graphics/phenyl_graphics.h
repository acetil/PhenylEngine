#pragma once

#include <memory>
#include <string>

#include "renderlayer/graphics_layer.h"
#include "ui/ui_manager.h"

#include "component/component.h"
#include "component/forward.h"
#include "common/events/entity_creation.h"
#include "util/optional.h"
#include "event/event_bus.h"

namespace graphics {
    namespace detail {
        class Graphics;
    }

    class Renderer;

    class PhenylGraphics {
    private:
        std::weak_ptr<detail::Graphics> graphics;
        std::shared_ptr<detail::Graphics> getGraphics () const;
    public:
        explicit PhenylGraphics (std::weak_ptr<detail::Graphics> graphics) : graphics{std::move(graphics)} {};

        [[nodiscard]] bool shouldClose () const;

        void pollEvents ();

        void render ();

        void sync (int fps);

        [[nodiscard]] double getDeltaTime () const;

        std::shared_ptr<GraphicsRenderLayer> getRenderLayer () const;

        Camera& getCamera ();

        void addEntityLayer (component::EntityComponentManager* componentManager);
        void onEntityCreation (event::EntityCreationEvent& event);

        Renderer* getRenderer () const; // TODO: remove

        //void setupWindowCallbacks (const event::EventBus::SharedPtr& bus);

        void deleteWindowCallbacks ();

        UIManager& getUIManager ();

        std::shared_ptr<detail::Graphics> tempGetGraphics () {
            return getGraphics();
        }

        void addEventHandlers (const event::EventBus::SharedPtr& eventBus);
        void updateUI ();

        void addComponentSerializers (component::EntitySerializer& serialiser);
    };

    class PhenylGraphicsHolder {
    private:
        std::shared_ptr<detail::Graphics> graphics;
    public:
        PhenylGraphicsHolder ();
        ~PhenylGraphicsHolder();

        PhenylGraphics getGraphics () const;

        std::shared_ptr<detail::Graphics> tempGetGraphics () const {
            return graphics;
        }
    };
}