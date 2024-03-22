#pragma once

#include <iosfwd>

#include "component/forward.h"
#include "forward.h"
#include "graphics/ui/forward.h"
#include "graphics/graphics_properties.h"
#include "runtime/runtime.h"

#include "component/component.h"
#include "component/component_serializer.h"

#include "logging/properties.h"
#include "graphics/camera.h"

namespace phenyl::engine {
    namespace detail {
        class Engine;
    }

    class ApplicationProperties {
    private:
        phenyl::graphics::GraphicsProperties graphicsProperties;
        logging::LoggingProperties loggingProperties;

        friend class detail::Engine;
        friend class PhenylEngine;
    public:
        ApplicationProperties () = default;

        ApplicationProperties& withResolution (const int width, const int height) {
            graphicsProperties.withWindowSize(width, height);

            return *this;
        }

        ApplicationProperties& withWindowTitle (std::string title) {
            graphicsProperties.withWindowTitle(std::move(title));

            return *this;
        }

        ApplicationProperties& withVsync (const bool vsync) {
            graphicsProperties.withVsync(vsync);

            return *this;
        }

        ApplicationProperties& withLogFile (std::string logFile) {
            loggingProperties.withLogFile(std::move(logFile));

            return *this;
        }

        ApplicationProperties& withLogLevel (const std::string& logger, const int level) {
            loggingProperties.withLogLevel(logger, level);

            return *this;
        }

        ApplicationProperties& withRootLogLevel (const int level) {
            loggingProperties.withRootLogLevel(level);

            return *this;
        }
    };

    class Application {
    private:
        ApplicationProperties properties;
        PhenylEngine* engine = nullptr;
        double targetFrameTime{1.0 / 60};
        double targetFps{60};
        double fixedTimeScale{1.0};

        friend class PhenylEngine;
        friend class detail::Engine;
    protected:
        component::ComponentManager& componentManager ();
        runtime::PhenylRuntime& runtime ();
        //graphics::Camera& camera ();
        //game::GameInput& input ();
        //graphics::UIManager& uiManager ();
        void setTargetFPS (double fps);
        void setFixedTimeScale (double newTimeScale);
    public:
        Application (ApplicationProperties properties);
        virtual ~Application() = default;

        virtual void init () = 0;
        virtual void update (double deltaTime) = 0;
        virtual void fixedUpdate (float deltaTime) = 0;
        virtual void shutdown () {

        }

        void pause ();
        void resume ();

        template <phenyl::common::CustomSerializable T>
        void addComponent () {
            runtime().template addComponent<T>();
        };

        template <typename T>
        void addUnserializedComponent () {
            runtime().template addUnserializedComponent<T>();
        }
    };
}