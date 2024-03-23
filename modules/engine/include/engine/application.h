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
#include "runtime/runtime.h"
#include "engine/plugins/default_2d_plugin.h"

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

    template <typename T>
    concept PluginType = std::derived_from<T, runtime::IPlugin> || std::derived_from<T, runtime::IInitPlugin>;

    class ApplicationBase {
    private:
        ApplicationProperties properties;
        runtime::PhenylRuntime* engineRuntime = nullptr;
        double targetFrameTime{1.0 / 60};
        double targetFps{60};
        double fixedTimeScale{1.0};

        friend class AppPlugin;
        template <PluginType ...Plugins>
        friend class Application;

        virtual void _init () = 0;
        ApplicationBase (ApplicationProperties properties);
    protected:
        component::ComponentManager& componentManager ();
        runtime::PhenylRuntime& runtime ();

        void setTargetFPS (double fps);
        void setFixedTimeScale (double newTimeScale);
    public:
        virtual ~ApplicationBase() = default;

        virtual void update (double deltaTime) = 0;
        virtual void fixedUpdate (double deltaTime) = 0;
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

        const ApplicationProperties& getProperties () const {
            return properties;
        }

        double getFixedTimeScale () const {
            return fixedTimeScale;
        }

        double getTargetFps () const {
            return targetFps;
        }
    };

    template <PluginType ...Plugins>
    class Application : public ApplicationBase {
    private:
        template <PluginType T, PluginType ...Args>
        static void InitPlugins (runtime::PhenylRuntime& runtime) {
            runtime.addPlugin<T>();

            if constexpr (sizeof...(Args) > 0) {
                InitPlugins<Args...>();
            }
        }

        void _init() final {
            InitPlugins<Plugins...>(runtime());

            init();
        }
    public:
        explicit Application (ApplicationProperties properties) : ApplicationBase{std::move(properties)} {}

        virtual void init () = 0;
    };

    template <>
    class Application<> : public ApplicationBase {
    private:
        void _init() final {
            init();
        }
    public:
        explicit Application (ApplicationProperties properties) : ApplicationBase{std::move(properties)} {}

        virtual void init () = 0;
    };
}