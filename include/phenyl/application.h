#pragma once

#include "properties.h"
#include "plugin.h"

#include "plugins/default_2d_plugin.h"

namespace phenyl {
    template <PluginType ...Plugins>
    class Application;

    namespace engine {
        class ApplicationBase {
        private:
            ApplicationProperties properties;
            runtime::PhenylRuntime* engineRuntime = nullptr;
            double targetFrameTime{1.0 / 60};
            double targetFps{60};
            double fixedTimeScale{1.0};

            virtual void _init () = 0;
            ApplicationBase (ApplicationProperties properties);

            friend class AppPlugin;
            template <PluginType ...Plugins>
            friend class ::phenyl::Application;
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
    }

    template <PluginType ...Plugins>
    class Application : public engine::ApplicationBase {
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
    class Application<> : public engine::ApplicationBase {
    private:
        void _init() final {
            init();
        }
    public:
        explicit Application (ApplicationProperties properties) : ApplicationBase{std::move(properties)} {}

        virtual void init () = 0;
    };

    using Application2D = Application<Default2DPlugin>;
}