#include <iostream>
#include <utility>
#include <phenyl/application.h>
#include <phenyl/asset.h>
#include <phenyl/input.h>
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
    std::vector<phenyl::Entity> entities;
    std::vector<float> rotationSpeeds;
    glm::vec3 axis{};
    float remainingTime = 3.0f;

    phenyl::Axis2DInput cameraControl;
    phenyl::InputAction zoomIn;
    phenyl::InputAction zoomOut;
public:
    explicit ModelViewer (phenyl::ApplicationProperties properties) : Application{std::move(properties)} {}

    void init () override {

    }

    void scene1 () {
        entities.emplace_back(runtime().world().create());

        entities.back().insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}.setScale(glm::vec3{0.2f})
        });
        entities.back().insert(phenyl::MeshRenderer3D{
            //.mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/cube.obj")
            .mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/suzanne.obj"),
            .material = phenyl::Assets::Load<phenyl::MaterialInstance>("resources/material_instances/mat1")
        });
        rotationSpeeds.emplace_back(60.0f * std::numbers::pi / 180);

        entities.emplace_back(runtime().world().create());
        entities.back().insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}.setScale(glm::vec3{0.2f}).translate(glm::vec3{0.8f, 0, -0.0f})
        });
        entities.back().insert(phenyl::MeshRenderer3D{
            .mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/cube.obj"),
            .material = phenyl::Assets::Load<phenyl::MaterialInstance>("resources/material_instances/mat2")
        });
        rotationSpeeds.emplace_back(-90.0f * std::numbers::pi / 180);

        // auto light1 = runtime().world().create();
        // light1.insert(phenyl::GlobalTransform3D{
        //     .transform = phenyl::Transform3D{}.translate(glm::vec3{-2, 0, -0})
        // });
        // light1.insert(phenyl::PointLight3D{
        //     .color = {1.0f, 1.0f, 1.0f},
        //     .brightness = 4.0f
        // });
        // //
        // auto light2 = runtime().world().create();
        // light2.insert(phenyl::GlobalTransform3D{
        //     .transform = phenyl::Transform3D{}.translate(glm::vec3{2, 0, -0})
        // });
        // light2.insert(phenyl::PointLight3D{
        //     .color = {1.0f, 0.2f, 0.2f},
        //     .brightness = 5.0f
        // });

        // auto light3 = runtime().world().create();
        // light3.insert(phenyl::GlobalTransform3D{
        //     .transform = phenyl::Transform3D{}.setRotation(phenyl::Quaternion::LookAt({1.0f, 0.0f, 0.0f}))
        // });
        // light3.insert(phenyl::DirectionalLight3D{
        //     .color = {1.0f, 1.0f, 1.0f},
        //     .brightness = 1,
        //     .castShadows = false
        // });
        //
        auto light4 = runtime().world().create();
        light4.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}
                         .setPosition({-1.0f, 0.0f, 0.0f})
                         .setRotation(phenyl::Quaternion::LookAt({-1.0f, 0.0f, 0.0f}))
        });
        light4.insert(phenyl::SpotLight3D{
            .color = {1.0f, 1.0f, 1.0f},
            .brightness = 10.0f,
            .innerAngle = 8.0f / 180.0f * std::numbers::pi,
            .outerAngle = 9.0f / 180.0f * std::numbers::pi,
            .castShadows = false
        });

        runtime().addSystem<phenyl::Update>("ModelViewer::rotate", this, &ModelViewer::rotate);
        runtime().resource<phenyl::Camera3D>().transform.translate(glm::vec3{0.0f, 0, -2.0f});
        chooseAxis();
    }

    void scene2 () {
        auto plane = runtime().world().create();
        plane.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}
                .withTranslation({0, -0.6f, 0})
                .withScale({25.0f, 0.2f, 25.0f})
        });
        plane.insert(phenyl::MeshRenderer3D{
            .mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/cube.obj"),
            .material = phenyl::Assets::Load<phenyl::MaterialInstance>("resources/material_instances/mat1")
        });

        auto cube1 = runtime().world().create();
        cube1.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}
                .withTranslation({0, 1.5f, 0})
                .withScale({0.5f, 0.5f, 0.5f})
        });
        cube1.insert(phenyl::MeshRenderer3D{
            .mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/cube.obj"),
            .material = phenyl::Assets::Load<phenyl::MaterialInstance>("resources/material_instances/mat2")
        });

        auto cube2 = runtime().world().create();
        cube2.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}
                .withTranslation({2.0f, 0.0f, 1.0f})
                .withScale({0.5f, 0.5f, 0.5f})
        });
        cube2.insert(phenyl::MeshRenderer3D{
            .mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/cube.obj"),
            .material = phenyl::Assets::Load<phenyl::MaterialInstance>("resources/material_instances/mat2")
        });

        auto cube3 = runtime().world().create();
        cube3.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}
                .withTranslation({-1.0f, 0.0f, 2.0f})
                .withScale({0.5f, 0.5f, 0.5f})
                .withRotation(phenyl::Quaternion::Rotation(glm::normalize(glm::vec3{1.0, 0.0, 1.0}), 60.0f * std::numbers::pi / 180.0f))
        });
        cube3.insert(phenyl::MeshRenderer3D{
            .mesh = phenyl::Assets::Load<phenyl::Mesh3D>("resources/meshes/cube.obj"),
            .material = phenyl::Assets::Load<phenyl::MaterialInstance>("resources/material_instances/mat2")
        });

        auto light = runtime().world().create();
        light.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}
            .setPosition({6, 6.0f, -3})
            .setRotation(phenyl::Quaternion::LookAt(-glm::vec3{-2.0f, -1.5f, 1.0f}))
        });

        light.insert(phenyl::SpotLight3D{
            .color = {1.0f, 1.0f, 1.0f},
            .brightness = 10.0f,
            .innerAngle = 20.0f * std::numbers::pi / 180.0f,
            .outerAngle = 21.0f * std::numbers::pi / 180.0f,
            .castShadows = true
        });
        // light.insert(phenyl::PointLight3D{
        //     .color = {1.0f, 1.0f, 1.0f},
        //     .brightness = 20.0f
        // });
        // light.insert(phenyl::DirectionalLight3D{
        //     .color = {1.0f, 1.0f, 1.0f},
        //     .brightness = 0.4f,
        //     .castShadows = true
        // });

        auto light2 = runtime().world().create();
        light2.insert(phenyl::GlobalTransform3D{
            .transform = phenyl::Transform3D{}
            .setPosition({0, 8.0f, -7})
            .setRotation(phenyl::Quaternion::LookAt(-glm::vec3{0.0f, -7.0f, 6.0f}))
        });

        light2.insert(phenyl::SpotLight3D{
            .color = {1.0f, 0.0f, 0.0f},
            .brightness = 25.0f,
            .innerAngle = 10.0f * std::numbers::pi / 180.0f,
            .outerAngle = 11.0f * std::numbers::pi / 180.0f,
            .castShadows = true
        });

        runtime().resource<phenyl::Camera3D>().transform
            .translate(glm::vec3{0.0f, 4.0f, 10.0f});
        runtime().resource<phenyl::Camera3D>().lookAt({0, 0, 0});

        // runtime().resource<phenyl::Camera3D>().transform
        //     .translate(glm::vec3{0.0f, 4.0f, 0.0f})
        //     .setRotation(phenyl::Quaternion::LookAt(-glm::vec3{1.0f, -2.0f, 0.0f}, phenyl::Quaternion::ForwardVector));
        // runtime().resource<phenyl::Camera3D>().fov = 40.0f;
        //runtime().resource<phenyl::Camera3D>().lookAt({0, 0, 0});
    }

    void postInit() override {
        //scene1();
        scene2();

        auto& input = runtime().resource<phenyl::GameInput>();
        cameraControl = input.addAxis2D("camera_control", true);
        zoomIn = input.addAction("zoom_in");
        zoomOut = input.addAction("zoom_out");

        input.addButtonAxis2DBinding("camera_control", "keyboard.key_a", glm::vec2{-1, 0});
        input.addButtonAxis2DBinding("camera_control", "keyboard.key_d", glm::vec2{1, 0});
        input.addButtonAxis2DBinding("camera_control", "keyboard.key_w", glm::vec2{0, 1});
        input.addButtonAxis2DBinding("camera_control", "keyboard.key_s", glm::vec2{0, -1});

        input.addActionBinding("zoom_in", "keyboard.key_q");
        input.addActionBinding("zoom_out", "keyboard.key_e");
        runtime().addSystem<phenyl::Update>("ModelViewer::mouseUpdate", this, &ModelViewer::updateMouse);
    }

    void rotate () {
        const auto& deltaTime = runtime().resource<phenyl::DeltaTime>();

        for (std::size_t i = 0; i < entities.size(); i++) {
            entities[i].get<phenyl::GlobalTransform3D>()->transform
                .rotateBy(phenyl::Quaternion::Rotation(axis, static_cast<float>(rotationSpeeds[i] * deltaTime())));
        }

        remainingTime -= deltaTime();
        if (remainingTime <= 0.0f) {
            chooseAxis();
        }
    }

    void updateMouse () {
        const auto& deltaTime = runtime().resource<const phenyl::DeltaTime>();
        auto& camera = runtime().resource<phenyl::Camera3D>();

        glm::vec2 axisSpeed = {20.0f, -10.0f};

        glm::vec3 zoom{};
        if (zoomIn.value()) {
            zoom += camera.transform.rotation() * glm::vec3{0, 0, -1};
        }

        if (zoomOut.value()) {
            zoom += camera.transform.rotation() * glm::vec3{0, 0, 1};
        }

        zoom *= 10.0f;

        auto vec = camera.transform.rotation() * glm::vec3{cameraControl.value() * axisSpeed, 0.0f};

        camera.transform.translate((vec + zoom) * static_cast<float>(deltaTime()));
        camera.lookAt({0, 0, 0});
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