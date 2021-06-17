#include <stdlib.h>

#include <string>
#include <utility>
#include <vector>
#include <utility>
#include <unordered_map>
#include <optional>
#include <logging/logging.h>
#include <component/component.h>
#include <event/events/entity_creation.h>
#include "textures/texture_atlas.h"
#include "renderlayer/render_layer.h"
#include "graphics_new_include.h"
#include "renderlayer/graphics_layer.h"
#include "graphics/renderers/renderer.h"
#include "font/glyph_atlas.h"
#include "ui/ui_manager.h"

#include "util/smart_help.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H
namespace graphics {

    class GraphicsData {
    public:

    };
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

class Graphics : public util::SmartHelper<Graphics> {
    private:
        Renderer* renderer;

        double lastTime;
        double deltaTime;

        std::shared_ptr<GraphicsRenderLayer> renderLayer;

        std::unordered_map<std::string, TextureAtlas> atlases;


        Camera camera;

        GlyphAtlas glyphAtlas;

        UIManager uiManager;

    public:
        explicit Graphics (Renderer* renderer, FontManager& manager);
        ~Graphics () {
            delete renderer;
        }
        bool shouldClose ();
        void pollEvents ();
        void render ();
        //void addShader (std::string name, ShaderProgram* program);
        void initTextureAtlas (const std::string& atlasName, const std::vector<Model>& images);
        std::optional<TextureAtlas> getTextureAtlas (const std::string& atlas);
        void sync (int fps);
        double getDeltaTime () const;
        std::shared_ptr<GraphicsRenderLayer> getRenderLayer ();
        //int createLayer (RenderLayer* layer);
        //RenderLayer* getLayer (std::string name);
        //virtual RenderLayer* getLayer (int layer);
        [[maybe_unused]] Camera& getCamera ();
        void addEntityLayer (component::EntityComponentManager::SharedPtr compManager);
        void onEntityCreation (event::EntityCreationEvent& event);
        Renderer* getRenderer () {
            return renderer; // TODO: remove
        }
        /*void addGlyphAtlas (GlyphAtlas& atlas) {
            glyphAtlas = std::move(atlas);
            renderLayer->addTex(glyphAtlas.getTex());
        }*/
        void setupWindowCallbacks (event::EventBus::SharedPtr bus);

        void deleteWindowCallbacks ();

        UIManager& getUIManager ();
    };
}

#endif //GRAPHICS_NEW_H
