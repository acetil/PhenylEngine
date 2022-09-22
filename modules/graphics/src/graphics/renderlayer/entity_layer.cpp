#include "../../../../physics/include/physics/components/simple_friction.h"
#include "entity_layer.h"
#include "component/components/2D/position.h"
#include "component/components/2D/rotation.h"

#include <utility>


#define BUFFER_SIZE 100 * 2 * 6

#define NUM_TRIANGLE_VERTICES 3
//#define TRIANGLES_PER_SPRITE 2
//#define NUM_POS_PER_VERTEX 2

using namespace graphics;

static void bufferPosDataNew (const component::EntityComponentManager::SharedPtr& manager, Buffer<glm::vec2>& buffer);
static void bufferUvDataNew (const component::EntityComponentManager::SharedPtr& manager, Buffer<glm::vec2> buffer);
static void bufferActualPosDataNew (const component::EntityComponentManager::SharedPtr& manager, Buffer<glm::vec2>& offsetBuffer, Buffer<glm::mat2>& transformBuffer);

namespace graphics {
    class EntityPipeline : public Pipeline<component::EntityComponentManager::SharedPtr> {
    private:
        PipelineStage renderStage;
        ShaderProgramNew shader;
        Buffer<glm::vec2> posBuffer;
        Buffer<glm::vec2> uvBuffer;
        Buffer<glm::vec2> offsetBuffer;
        Buffer<glm::mat2> transformBuffer;

    public:
        EntityPipeline () = default;

        explicit EntityPipeline (const ShaderProgramNew& _shader) : shader{_shader} {};

        void init (Renderer* renderer) override {
            renderStage = std::move(renderer->buildPipelineStage(PipelineStageBuilder(shader)
                                                               .addVertexAttrib<glm::vec2>(0)
                                                               .addVertexAttrib<glm::vec2>(1)
                                                               .addVertexAttrib<glm::vec2>(2)
                                                               .addVertexAttrib<glm::mat2>(3)));

            posBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
            uvBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
            offsetBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
            transformBuffer = renderer->makeBuffer<glm::mat2>(BUFFER_SIZE);

            renderStage.bindBuffer(0, posBuffer);
            renderStage.bindBuffer(1, uvBuffer);
            renderStage.bindBuffer(2, offsetBuffer);
            renderStage.bindBuffer(3, transformBuffer);
        }

        void applyCamera (graphics::Camera& camera) {
            renderStage.applyUniform(camera.getUniformName(), camera.getCamMatrix());
        }

        void bufferData (component::EntityComponentManager::SharedPtr& manager) override {
            /*posBuffer.clearData();
            uvBuffer.clearData();
            offsetBuffer.clearData();
            transformBuffer.clearData();*/
            renderStage.clearBuffers();


            bufferPosDataNew(manager, posBuffer);
            bufferUvDataNew(manager, uvBuffer);
            bufferActualPosDataNew(manager, offsetBuffer, transformBuffer);

            /*posBuffer.bufferData();
            uvBuffer.bufferData();
            offsetBuffer.bufferData();
            transformBuffer.bufferData();*/
            renderStage.bufferAllData();
        }

        void render () override {
            renderStage.render();
        }
    };
}


//void bufferPosData (Model2D* comp, int numEntities, [[maybe_unused]] int direction, std::pair<Buffer*, Transform2D*> tup);
//static void bufferPosData (const component::EntityComponentManager::SharedPtr& manager, Buffer* buffers);
//void bufferUvData (Model2D* comp, int numEntities, [[maybe_unused]] int direction, Buffer* buf);
//static void bufferUvData (const component::EntityComponentManager::SharedPtr& manager, Buffer* buffer);
//void bufferActualPosData (Transform2D* comp, int numEntities, [[maybe_unused]] int direction, Buffer* bufs);
//static void bufferActualPosData (const component::EntityComponentManager::SharedPtr& manager, Buffer* buffer);
std::string EntityRenderLayer::getName () {
    return "entity_layer";
}

int EntityRenderLayer::getPriority () {
    return 1;
}

bool graphics::EntityRenderLayer::isActive () {
    return active;
}

void EntityRenderLayer::gatherData () {

}

void EntityRenderLayer::preRender (graphics::Renderer* renderer) {
    /*componentManager->applyFunc<Model2D>(bufferPosData, std::pair(&buffers[0],
                                                            componentManager->getComponent<Transform2D>().orElse(nullptr)));*/
    //bufferPosData(componentManager, &buffers[0]);

    //componentManager->applyFunc<Model2D>(bufferUvData, &buffers[1]);

    //bufferUvData(componentManager, &buffers[1]);

    /*for (auto i : *componentManager) {
        i.applyFunc<Transform2D, component::Rotation2D>([](Transform2D& absPos, component::Rotation2D& rotComp) {
            absPos.transform *= rotComp.rotMatrix;
        });
    }*/
    for (const auto& i : componentManager->getConstrainedView<Transform2D, component::Rotation2D>()) {
        i.get<Transform2D>().rotTransform = i.get<Transform2D>().transform * i.get<component::Rotation2D>().rotMatrix;
    }

    /*componentManager->applyFunc<Transform2D, component::Rotation2D>([](Transform2D* absPos, component::Rotation2D* rotComp, int numEntities, int) {
        for (int i = 0; i < numEntities; i++) {
            absPos[i].transform *= rotComp[i].rotMatrix;
        }
    });*/

    //componentManager->applyFunc<Transform2D>(bufferActualPosData, &buffers[2]);
    //bufferActualPosData(componentManager, &buffers[2]);

    entityPipeline->bufferData(componentManager);

    /*componentManager->applyFunc<Transform2D, component::Rotation2D>([](Transform2D* absPos, component::Rotation2D* rotComp, int numEntities, int) {
        for (int i = 0; i < numEntities; i++) {
            absPos[i].transform *= glm::inverse(rotComp[i].rotMatrix);
        }
    });*/
    /*for (auto i : *componentManager) {
        i.applyFunc<Transform2D, component::Rotation2D>([](Transform2D& absPos, component::Rotation2D& rotComp) {
            absPos.transform *= glm::inverse(rotComp.rotMatrix);
        });
    }*/
    /*for (const auto& i :  componentManager->getConstrainedView<Transform2D, component::Rotation2D>()) {
        i.get<Transform2D>().transform *= glm::inverse(i.get<component::Rotation2D>().rotMatrix);
    }*/

    //numTriangles = buffers[0].currentSize() / sizeof(float) / 2 / 3;
    //renderer->bufferData(buffIds, buffers);
}

int EntityRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void EntityRenderLayer::applyUniform (int uniformId, void* data) {

}

void EntityRenderLayer::applyCamera (graphics::Camera camera) {
    //shaderProgram.bind();
    //shaderProgram.applyUniform(camera.getUniformName(), camera.getCamMatrix());
    entityPipeline->applyCamera(camera);
}

void EntityRenderLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {
    //shaderProgram.bind();
    //frameBuf->bind();
    //printf("Num triangles: %d\n", numTriangles);
    //renderer->render(buffIds, shaderProgram, numTriangles); // TODO: Make better
    entityPipeline->render();
}

EntityRenderLayer::EntityRenderLayer (graphics::Renderer* renderer,
                                                component::EntityComponentManager::SharedPtr componentManager) : shaderProgram{renderer->getProgramNew("default").orThrow()} {
    this->componentManager = std::move(componentManager);
    //this->shaderProgram = renderer->getProgram("entity").value();
    //this->shaderProgram = renderer->getProgramNew("default").orThrow(); // TODO
    /*this->buffIds = renderer->getBufferIds(5, BUFFER_SIZE * 2 * sizeof(float), {2, 2, 2, 2, 2});
    this->buffers[0] = Buffer(BUFFER_SIZE * 2, sizeof(float), false);
    this->buffers[1] = Buffer(BUFFER_SIZE * 2, sizeof(float), false);
    this->buffers[2] = Buffer(BUFFER_SIZE * 2, sizeof(float), false);
    this->buffers[3] = Buffer(BUFFER_SIZE * 4, sizeof(float), false);
    this->buffers[4] = Buffer(BUFFER_SIZE * 4, sizeof(float), false); // TODO: better way to input matrices*/

    this->entityPipeline = std::make_unique<EntityPipeline>(shaderProgram);
    this->entityPipeline->init(renderer);
}

EntityRenderLayer::~EntityRenderLayer () = default;

/*void bufferPosData (Model2D* comp, int numEntities, [[maybe_unused]] int direction, std::pair<Buffer*, Transform2D*> tup) {
    auto [buf, modComp] = tup;
    for (int i = 0; i < numEntities; i++) {
        //logging::logf(LEVEL_DEBUG, "Num vertices: %d", comp[i].positionData.size());
        //logging::log(LEVEL_DEBUG, "Buffering entity pos data!");
        //auto ptr = buffer->getVertexBufferPos();
        /*glm::vec2 vertices[4];
        vertices[0] = comp->pos;
        vertices[1] = comp->pos + comp->vec1;
        vertices[2] = comp->pos + comp->vec2;
        vertices[3] = comp->pos + comp->vec1 + comp->vec2;
        for (int j = 0; j < NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE; j++) {
            int correctedVertex = j % NUM_TRIANGLE_VERTICES + j / NUM_TRIANGLE_VERTICES;
            buffer->pushData(&vertices[correctedVertex].x, 2);
        } *//*

        buf->pushData(comp->positionData.begin(), comp->positionData.size());
        modComp->vertices = comp->positionData.size() / 2;
        comp++;
        modComp++;
    }

}*/
static void bufferPosDataNew (const component::EntityComponentManager::SharedPtr& manager, Buffer<glm::vec2>& buffer) {
    for (const auto& i : manager->getConstrainedView<Model2D, Transform2D>()) {
        auto& model = i.get<Model2D>();
        buffer.pushData(model.positionData.begin(), model.positionData.end());
        i.get<Transform2D>().vertices = model.positionData.size();
    }
}
/*void bufferUvData (Model2D* comp, int numEntities, [[maybe_unused]] int direction, Buffer* buf) {
    for (int i = 0; i < numEntities; i++) {
        buf->pushData(comp->uvData.begin(), comp->uvData.size());
        comp++;
    }
}*/

static void bufferUvDataNew (const component::EntityComponentManager::SharedPtr& manager, Buffer<glm::vec2> buffer) {
    for (const auto& i : manager->getConstrainedView<Model2D>()) {
        buffer.pushData(i.get<Model2D>().uvData.begin(), i.get<Model2D>().uvData.end());
    }
}
/*void bufferActualPosData (Transform2D* comp, int numEntities, [[maybe_unused]] int direction, Buffer* bufs) {
    for (int i = 0; i < numEntities; i++) {
        //logging::logf(LEVEL_DEBUG, "Abs pos: <%f, %f>", comp->pos.x, comp->pos.y);
        for (int j = 0; j < comp->vertices; j++) {
            bufs->pushData(&comp->pos[0], 2);
            (bufs + 1)->pushData(&comp->transform[0][0], 2);
            (bufs + 2)->pushData(&comp->transform[1][0], 2);
            //buf.second->pushData(&comp->transform[2], 2); // TODO: update buffer implementation
        }
        comp++;
    }
}*/

static void bufferActualPosDataNew (const component::EntityComponentManager::SharedPtr& manager, Buffer<glm::vec2>& offsetBuffer, Buffer<glm::mat2>& transformBuffer) {
    for (const auto& i : manager->getConstrainedView<Transform2D, component::Position2D>()) {
        auto& absPos = i.get<Transform2D>();
        auto& posComp = i.get<component::Position2D>();
        for (int j = 0; j < absPos.vertices; j++) {
            offsetBuffer.pushData(posComp.get());
            //(buffer + 1)->pushData(&absPos.transform[0][0], 2);
            //(buffer + 2)->pushData(&absPos.transform[1][0], 2); // TODO: update buffer implementation
            transformBuffer.pushData(absPos.rotTransform);
        }
    }
}