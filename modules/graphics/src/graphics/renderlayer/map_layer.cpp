#include "graphics/renderlayer/map_layer.h"

#include <utility>

#define BUFFER_SIZE 100

using namespace graphics;

class MapPipelineInt : public MapPipeline {
private:
    PipelineStage mapRenderStage;
    ShaderProgramNew shader;

    Buffer<glm::vec2> posBuffer;
    Buffer<glm::vec2> uvBuffer;
    Buffer<glm::vec2> offsetBuffer;
    Buffer<glm::mat2> transformBuffer;
public:
    MapPipelineInt (const ShaderProgramNew& _shader) : shader{_shader} {}
    void init (Renderer* renderer) override {
        mapRenderStage = std::move(renderer->buildPipelineStage(PipelineStageBuilder(shader)
                .addVertexAttrib<glm::vec2>(0)
                .addVertexAttrib<glm::vec2>(1)
                .addVertexAttrib<glm::vec2>(2)
                .addVertexAttrib<glm::mat2>(3)));

        posBuffer = renderer->makeBuffer<glm::vec2>(0);
        uvBuffer = renderer->makeBuffer<glm::vec2>(0);
        offsetBuffer = renderer->makeBuffer<glm::vec2>(0);
        transformBuffer = renderer->makeBuffer<glm::mat2>(0);

        mapRenderStage.bindBuffer(0, posBuffer);
        mapRenderStage.bindBuffer(1, uvBuffer);
        mapRenderStage.bindBuffer(2, offsetBuffer);
        mapRenderStage.bindBuffer(3, transformBuffer);
    }

    void bufferData (game::Map::SharedPtr& map) override {
        mapRenderStage.clearBuffers();
        auto models = map->getModels();
        std::size_t numVertices = 0;

        for (const auto& m : models) {
            numVertices += std::get<AbsolutePosition>(m).vertices;
        }

        posBuffer.resizeBuffer(numVertices);
        uvBuffer.resizeBuffer(numVertices);
        offsetBuffer.resizeBuffer(numVertices);
        transformBuffer.resizeBuffer(numVertices);

        for (const auto& m : models) {
            posBuffer.pushData(std::get<FixedModel>(m).positionData.cbegin(), std::get<FixedModel>(m).positionData.cend());
            uvBuffer.pushData(std::get<FixedModel>(m).uvData.cbegin(), std::get<FixedModel>(m).uvData.cend());

            for (std::size_t i = 0; i < std::get<AbsolutePosition>(m).vertices; i++) {
                offsetBuffer.pushData(std::get<glm::vec2>(m));
                transformBuffer.pushData(std::get<AbsolutePosition>(m).transform);
            }
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
    //program = renderer->getProgramNew("default").orThrow();
    //this->atlas = atlas;
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
    /*if (needDataBuffer) {
        auto models = map->getModels();
        int numVertices = 0;
        for (const auto& m : models) {
            numVertices += m.first.vertices;
        }
        bufferIds = renderer->getBufferIds(5, numVertices * 2 * sizeof(float), {2, 2, 2, 2, 2});
        buffers[0] = Buffer(numVertices * 2, sizeof(float), true);
        buffers[1] = Buffer(numVertices * 2, sizeof(float), true);
        buffers[2] = Buffer(numVertices * 2, sizeof(float), true);
        buffers[3] = Buffer(numVertices * 2, sizeof(float), true);
        buffers[4] = Buffer(numVertices * 2, sizeof(float), true);
        for (auto& m : models) {
            buffers[0].pushData((float*)m.second.positionData.begin(), m.second.positionData.size() * 2);
            buffers[1].pushData((float*)m.second.uvData.begin(), m.second.uvData.size() * 2);
            for (int i = 0; i < m.first.vertices; i++) {
                buffers[2].pushData(&m.first.pos[0], 2);
                buffers[3].pushData(&m.first.transform[0][0], 2);
                buffers[4].pushData(&m.first.transform[1][0], 2);
            }
        }
        logging::log(LEVEL_DEBUG, "Sizes: {} {} {} {} {}", buffers[0].currentSize(), buffers[1].currentSize(), buffers[2].currentSize(),
                      buffers[3].currentSize(), buffers[4].currentSize());
        renderer->bufferData(bufferIds, buffers);
        needDataBuffer = false;
        numTriangles = numVertices / 3;
        logging::log(LEVEL_DEBUG, "Buffered map data with {} vertices!", numVertices);

    }*/
    /*if (requiresBuffer) {
        bufferIds = renderer->getBufferIds(2, map->getNumTileVertices() * 6 * sizeof(float));
        requiresBuffer = false;
    }
    if (needDataBuffer) {
        int numVertices = map->getNumTileVertices();
        buffers[0] = Buffer(numVertices * 2 , sizeof(float), true);
        buffers[1] = Buffer(numVertices * 2, sizeof(float), true);
        float* vertexData = map->getTileVertices();
        float* uvData = map->getTileUvs();
        buffers[0].pushData(vertexData, numVertices * 2);
        buffers[1].pushData(uvData, numVertices * 2);
        renderer->bufferData(bufferIds, buffers);
        needDataBuffer = false;
        numTriangles = numVertices / 3;
        delete[] vertexData;
        delete[] uvData;
        logging::logf(LEVEL_DEBUG, "Buffered map data with %d vertices!", numVertices);
    }*/
}

int MapRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void MapRenderLayer::applyUniform (int uniformId, void* data) {

}

void MapRenderLayer::applyCamera (Camera camera) {
    //program.bind();
    //program.applyUniform(camera.getUniformName(), camera.getCamMatrix());
    mapPipeline->applyCamera(camera);
}

void MapRenderLayer::render (Renderer* renderer, FrameBuffer* frameBuf) {
    /*program.bind();
    frameBuf->bind();
    renderer->render(bufferIds, program, numTriangles);*/
    mapPipeline->render();
}

void MapRenderLayer::attachMap (game::Map::SharedPtr _map) {
    /*logging::log(LEVEL_DEBUG, "Map attached!");
    this->map = std::move(_map);
    this->map->setAtlas(atlas);
    active = true;
    needDataBuffer = true;*/
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



