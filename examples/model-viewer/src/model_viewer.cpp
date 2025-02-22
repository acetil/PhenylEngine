#include <iostream>
#include <phenyl/application.h>
#include <phenyl/asset.h>
#include <phenyl/transform.h>
#include <phenyl/world.h>

#include <phenyl/components/3D/global_transform.h>
#include <phenyl/components/3D/mesh_renderer.h>

#include <phenyl/graphics/mesh.h>

#include <phenyl/maths.h>

#include "phenyl/entrypoint.h"
#include "phenyl/components/3D/lighting.h"
#include "phenyl/graphics/material.h"
#include "util/random.h"

class ModelViewer : public phenyl::Application3D {
private:
    phenyl::Entity entity;
    phenyl::Entity entity2;
    glm::vec3 axis{};
    float remainingTime = 3.0f;
public:
    explicit ModelViewer (const phenyl::ApplicationProperties& properties) : Application{properties} {}

    void init () override {

    }

    void postInit() override {
        entity = runtime().world().create();

        entity.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}.setScale(glm::vec3{0.2f})
        });
        entity.insert(phenyl::MeshRenderer3D{
            //.mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/cube.obj")
            .mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/suzanne.obj"),
            .material = phenyl::Assets::Load<phenyl::MaterialInstance>("resources/material_instances/mat1")
        });

        entity2 = runtime().world().create();
        entity2.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}.setScale(glm::vec3{0.2f}).translate(glm::vec3{0.5f, 0, -0.4f})
        });
        entity2.insert(phenyl::MeshRenderer3D{
            .mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/cube.obj"),
            .material = phenyl::Assets::Load<phenyl::MaterialInstance>("resources/material_instances/mat2")
        });

        auto light1 = runtime().world().create();
        light1.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}.translate(glm::vec3{-2, 0, -0})
        });
        light1.insert(phenyl::PointLight3D{
            .color = {1.0f, 1.0f, 1.0f},
            .brightness = 4.0f
        });

        auto light2 = runtime().world().create();
        light2.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}.translate(glm::vec3{2, 0, -0})
        });
        light2.insert(phenyl::PointLight3D{
            .color = {1.0f, 0.2f, 0.2f},
            .brightness = 5.0f
        });

        auto light3 = runtime().world().create();
        light3.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}.setRotation(phenyl::Quaternion::LookAt({0.0f, 1.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f}))
        });
        light3.insert(phenyl::DirectionalLight3D{
            .color = {1.0f, 1.0f, 1.0f},
            .brightness = 5.0f
        });

        auto light4 = runtime().world().create();
        light4.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}
                .setPosition({-1.0f, -1.0f, 0.0f})
                .setRotation(phenyl::Quaternion::LookAt({1.0f, 1.0f, 0.0f}))
        });
        light4.insert(phenyl::SpotLight3D{
            .color = {1.0f, 1.0f, 1.0f},
            .brightness = 20.0f
        });

        runtime().addSystem<phenyl::Update>("ModelViewer::rotate", this, &ModelViewer::rotate);

        runtime().resource<phenyl::Camera3D>().transform.translate(glm::vec3{0.0f, 0, 2.0f});
        chooseAxis();
    }

    void rotate () {
        const auto& deltaTime = runtime().resource<phenyl::DeltaTime>();

        entity.get<phenyl::GlobalTransform3D>()->transform
            .rotateBy(phenyl::Quaternion::Rotation(axis, static_cast<float>(60.0 * deltaTime() * std::numbers::pi / 180)));

        entity2.get<phenyl::GlobalTransform3D>()->transform
            .rotateBy(phenyl::Quaternion::Rotation(axis, static_cast<float>(-90.0 * deltaTime() * std::numbers::pi / 180)));

        remainingTime -= deltaTime();
        if (remainingTime <= 0.0f) {
            chooseAxis();
        }

        //runtime().resource<phenyl::Camera3D>().transform.translate(static_cast<float>(deltaTime()) * glm::vec3{-0.1f, 0, 0.0f});

        // auto view = runtime().resource<phenyl::Camera3D>().viewMatrix();
        // auto proj = runtime().resource<phenyl::Camera3D>().perspectiveMatrix();
        // auto cam = runtime().resource<phenyl::Camera3D>().cameraMatrix();
        // auto model = entity.get<phenyl::GlobalTransform3D>()->transform.transformMatrx();
        // std::cout << std::format("Model: <\n\t{},\n\t{},\n\t{},\n\t{}\n>\n", model[0], model[1], model[2], model[3]);
        // std::cout << std::format("View: <\n\t{},\n\t{},\n\t{},\n\t{}\n>\n", view[0], view[1], view[2], view[3]);
        // std::cout << std::format("Proj: <\n\t{},\n\t{},\n\t{},\n\t{}\n>\n", proj[0], proj[1], proj[2], proj[3]);
        // std::cout << std::format("Camera: <\n\t{},\n\t{},\n\t{},\n\t{}\n>\n", cam[0], cam[1], cam[2], cam[3]);
        //
        // auto total = cam * model;
        // std::cout << std::format("Total: <\n\t{},\n\t{},\n\t{},\n\t{}\n>\n", total[0], total[1], total[2], total[3]);
        //
        // std::cout << std::format("Applied: right={}, up={}, depth={}\n", total * glm::vec4{1, 0, 0, 1}, total * glm::vec4{0, -1, 0, 1}, total * glm::vec4{0, 0, 1, 1});
    }

    void chooseAxis () {
        auto x = phenyl::util::Random::Rand(-1.0f, 1.0f);
        auto remaining = std::sqrt(1.0f - x * x);
        auto y = phenyl::util::Random::Rand(-remaining, remaining);
        auto z = std::sqrt(1.0f - x * x - y * y);

        axis = glm::normalize(glm::vec3{x, y, z});
        remainingTime = 2.5f;
    }
};

PHENYL_ENTRYPOINT(ModelViewer)