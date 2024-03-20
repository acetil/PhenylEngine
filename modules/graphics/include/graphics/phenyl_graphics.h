#pragma once

#include <memory>
#include <string>

#include "renderlayer/graphics_layer.h"
#include "ui/ui_manager.h"
#include "graphics_properties.h"

#include "component/component.h"
#include "component/forward.h"
#include "util/optional.h"

namespace phenyl::graphics {
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

        Renderer* getRenderer () const; // TODO: remove

        //void setupWindowCallbacks (const event::EventBus::SharedPtr& bus);

        void deleteWindowCallbacks ();

        UIManager& getUIManager ();

        std::shared_ptr<detail::Graphics> tempGetGraphics () {
            return getGraphics();
        }
        void updateUI ();
        void frameUpdate (component::ComponentManager& componentManager);

        void addComponentSerializers (component::EntitySerializer& serialiser);
    };

    class PhenylGraphicsHolder {
    private:
        std::shared_ptr<detail::Graphics> graphics;
    public:
        PhenylGraphicsHolder (const GraphicsProperties& properties);
        ~PhenylGraphicsHolder();

        PhenylGraphics getGraphics () const;

        std::shared_ptr<detail::Graphics> tempGetGraphics () const {
            return graphics;
        }
    };

    std::unique_ptr<detail::Graphics> MakeGraphics (const GraphicsProperties& properties);
}