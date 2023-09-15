#pragma once

#include <stdlib.h>

#include <string>
#include <utility>
#include <vector>
#include <utility>
#include <unordered_map>
#include <optional>
#include "logging/logging.h"
#include "component/component.h"
#include "common/events/entity_creation.h"
#include "graphics/renderlayer/render_layer.h"
#include "graphics/renderlayer/graphics_layer.h"
#include "graphics/renderers/renderer.h"
#include "graphics/font/glyph_atlas.h"
#include "graphics/ui/ui_manager.h"
#include "graphics/phenyl_graphics.h"
#include "graphics/textures/sprite_atlas.h"
#include "common/events/cursor_position_change.h"
#include "common/events/player_shoot_change.h"
#include "common/events/theme_change.h"
#include "common/events/debug/reload_theme.h"
#include "event/event.h"
#include "common/input/forward.h"
#include "util/smart_help.h"
#include "util/optional.h"

namespace graphics {

    class GraphicsData {
    public:

    };

    namespace detail {
        /*class RenderLayer {
                                    private:
                                        std::string name;
                                        ShaderProgram* program;
                                    public:
                                        RenderLayer (std::string name, ShaderProgram* program, TextureAtlas* atlas, int maxModels);
                                        std::string getName ();
                                        int getDataId (std::string dataName);
                                        void render ();
                                        int getUniformId (std::string uniformName);
                                        template<typename T>
                                        void applyUniform (int uniformId, T value);
                                        template<typename T>
                                        T* getDataPtr (int dataId, int modelNum);
                                        int getNumModels ();
                                        int addModel (int modelId);

                                    };*/

        class Graphics : public util::SmartHelper<Graphics, true> {
        private:
            event::EventScope eventScope;
            std::unique_ptr<Renderer> renderer;

            double lastTime;
            double deltaTime;

            bool timeIsPaused;
            double pauseStartTime;

            std::shared_ptr<GraphicsRenderLayer> renderLayer;

            //std::unordered_map<std::string, TextureAtlas> atlases;

            std::vector<std::shared_ptr<common::ProxySource>> inputSources;

            Camera camera;

            GlyphAtlas glyphAtlas;

            UIManager uiManager;

        public:
            explicit Graphics (std::unique_ptr<Renderer> renderer, FontManager& manager);
            ~Graphics ();
            bool shouldClose ();
            void pollEvents ();
            void render ();
            //void addShader (std::string name, ShaderProgram* program);
            void sync (int fps);
            double getDeltaTime () const;
            std::shared_ptr<GraphicsRenderLayer> getRenderLayer ();
            //int createLayer (RenderLayer* layer);
            //RenderLayer* getLayer (std::string name);
            //virtual RenderLayer* getLayer (int layer);
            [[maybe_unused]] Camera& getCamera ();
            void addEntityLayer (component::EntityComponentManager* compManager);
            void onEntityCreation (event::EntityCreationEvent& event);
            void onMousePosChange (event::CursorPosChangeEvent& event);

            void onThemeReload (event::ReloadThemeEvent& event);
            void onThemeChange (event::ChangeThemeEvent& event);

            Renderer* getRenderer () {
                return renderer.get(); // TODO: remove
            }
            /*void addGlyphAtlas (GlyphAtlas& atlas) {
                glyphAtlas = std::move(atlas);
                renderLayer->addTex(glyphAtlas.getTex());
            }*/
            void setupWindowCallbacks (const event::EventBus::SharedPtr& bus);

            void deleteWindowCallbacks ();

            UIManager& getUIManager ();
            void updateUI ();

            void addEventHandlers (const event::EventBus::SharedPtr& eventBus);
            std::vector<std::shared_ptr<common::InputSource>> getInputSources ();

            void addComponents (component::ComponentManager& manager);
            void addComponentSerializers (component::EntitySerializer& serialiser);
        };
    }
}
