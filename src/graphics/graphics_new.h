#include <string>
#include <utility>
#include <vector>
#include <utility>

#ifndef GRAPHICS_NEW_H
#define GRAPHICS_NEW_H
namespace graphics {
    class FrameBuffer {
    public:
        virtual void bind () = 0;
    };
    class ShaderProgram {

    };
    class Image;
    class Model;
    class TextureAtlas;
    class RenderLayer;
    class Camera;
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
    struct BufferInfo {
        int numBuffers;
        int size;
        std::vector<int> elementSizes;
        BufferInfo (int numBuffers, int size, std::vector<int> elementSizes) {
            this->numBuffers = numBuffers;
            this->size = size;
            this->elementSizes = std::move(elementSizes);
        }
        BufferInfo () {
            this->numBuffers = 0;
            this->size = 0;
            this->elementSizes = std::vector<int>();
        }
        bool isEmpty () const {
            return numBuffers == 0 && size == 0;
        }
    };

    class Buffer {

    };
    class Renderer {
    public:

    };
    class RenderLayer {
    public:
        virtual std::string getName () = 0;
        virtual int getPriority () = 0;
        virtual bool isActive () = 0;
        virtual BufferInfo getBufferInfo () = 0;
        virtual void addBuffer (Buffer buf) = 0;
        virtual void gatherData () = 0;
        virtual void preRender (Renderer* renderer) = 0;
        virtual void getUniformId (std::string uniformName) = 0;
        virtual void applyUniform (int uniformId, void* data) = 0;
        virtual void applyCamera (Camera camera) = 0;
        virtual void render (Renderer* renderer, FrameBuffer* frameBuf) = 0;
    };



    class GraphicsNew {
    public:
        GraphicsNew (Renderer* renderer);
        bool shouldClose ();
        void pollEvents ();
        void render ();
        void addShader (std::string name, ShaderProgram* program);
        TextureAtlas* initTextureAtlas (std::string atlasName, std::vector<Model*> images);
        TextureAtlas* getTextureAtlas (std::string atlas);
        void sync (int fps);
        float getDeltaTime ();
        int createLayer (RenderLayer* layer);
        RenderLayer* getLayer (std::string name);
        virtual RenderLayer* getLayer (int layer);
        Camera getCamera ();
    };
}

#endif //GRAPHICS_NEW_H
