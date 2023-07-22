#include "graphics/renderlayer/map_layer.h"
#include "common/maths/2d/transform.h"

#include <utility>

#define BUFFER_SIZE 100

using namespace graphics;

class MapPipelineInt : public MapPipeline {
private:
    PipelineStage mapRenderStage;
    ShaderProgramNew shader;

    Buffer<glm::vec2> posBuffer;
    Buffer<glm::vec2> uvBuffer;
public:
    MapPipelineInt (const ShaderProgramNew& _shader) : shader{_shader} {}
    void init (Renderer* renderer) override {
        mapRenderStage = renderer->buildPipelineStage(PipelineStageBuilder(shader)
                .addVertexAttrib<glm::vec2>(0)
                .addVertexAttrib<glm::vec2>(1));

        posBuffer = renderer->makeBuffer<glm::vec2>(0);
        uvBuffer = renderer->makeBuffer<glm::vec2>(0);

        mapRenderStage.bindBuffer(0, posBuffer);
        mapRenderStage.bindBuffer(1, uvBuffer);
    }

    void bufferData (game::Map::SharedPtr& map) override {
        mapRenderStage.clearBuffers();
        auto models = map->getModels();
        std::size_t numVertices = 0;

        for (const auto& p : models) {
            numVertices += std::get<Model2D>(p).positionData.size();
        }

        posBuffer.resizeBuffer(numVertices);
        uvBuffer.resizeBuffer(numVertices);

        for (const auto& p : models) {
            const auto& model = std::get<Model2D>(p);
            const auto& transform = std::get<common::Transform2D>(p);
            for (auto i : model.positionData) {
                posBuffer.pushData(transform.apply(i));
            }

            uvBuffer.pushData(model.uvData.cbegin(), model.uvData.cend());
        }

        mapRenderStage.bufferAllData();
    }

    void applyCamera(const Camera& camera) override {
        mapRenderStage.applyUniform(graphics::Camera::getUniformName(), camera.getCamMatrix());
    }

    void render () override {
        mapRenderStage.render();
    }
};


MapRenderLayer::MapRenderLayer (Renderer* renderer, TextureAtlas& _atlas) : atlas(_atlas), program{renderer->getProgramNew("default").orThrow()} {
    map = nullptr;
    mapPipeline = std::make_unique<MapPipelineInt>(program);
    mapPipeline->init(renderer);
}

std::string MapRenderLayer::getName() {
    return "map_layer";
}

int MapRenderLayer::getPriority() {
    return 1;
}

bool MapRenderLayer::isActive() {
    return active;
}

void MapRenderLayer::gatherData () {
    
}

void MapRenderLayer::preRender (Renderer* renderer) {

}

int MapRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void MapRenderLayer::applyUniform (int uniformId, void* data) {

}

void MapRenderLayer::applyCamera (Camera camera) {
    mapPipeline->applyCamera(camera);
}

void MapRenderLayer::render (Renderer* renderer, FrameBuffer* frameBuf) {
    mapPipeline->render();
}

void MapRenderLayer::attachMap (game::Map::SharedPtr _map) {
    if (!_map) {
        active = false;
        return;
    }
    _map->setAtlas(atlas);
    mapPipeline->bufferData(_map);
    active = true;
}

void MapRenderLayer::onMapLoad (event::MapLoadEvent& event) {
    attachMap(event.map);
}

event::EventScope& MapRenderLayer::getEventScope () {
    return eventScope;
}



