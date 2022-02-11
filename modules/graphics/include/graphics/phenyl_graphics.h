#pragma once

#include <memory>
#include <string>

#include "graphics_new_include.h"
#include "textures/texture_atlas.h"
#include "renderlayer/graphics_layer.h"
#include "ui/ui_manager.h"

#include "component/component.h"
#include "event/events/entity_creation.h"
#include "util/optional.h"

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

        void initTextureAtlas (const std::string& atlasName, const std::vector<Model>& images);
        util::Optional<graphics::TextureAtlas&> getTextureAtlas (const std::string& atlas) const;

        void sync (int fps);

        [[nodiscard]] double getDeltaTime () const;

        std::shared_ptr<GraphicsRenderLayer> getRenderLayer () const;

        Camera& getCamera ();

        void addEntityLayer (component::EntityComponentManager::SharedPtr componentManager);
        void onEntityCreation (event::EntityCreationEvent& event);

        Renderer* getRenderer () const; // TODO: remove

        //void setupWindowCallbacks (const event::EventBus::SharedPtr& bus);

        void deleteWindowCallbacks ();

        UIManager& getUIManager ();

        std::shared_ptr<detail::Graphics> tempGetGraphics () {
            return getGraphics();
        }

        void addEventHandlers (const event::EventBus::SharedPtr& eventBus);
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