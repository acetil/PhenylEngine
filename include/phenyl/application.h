#pragma once

#include "properties.h"
#include "plugin.h"

#include "plugins/default_2d_plugin.h"
#include "plugins/default_3d_plugin.h"

namespace phenyl {
    template <PluginType ...Plugins>
    class Application;

    namespace engine {
        class AppPlugin;


        class ApplicationBase {
        public:
            virtual ~ApplicationBase() = default;

            virtual void postInit () {}
            virtual void shutdown () {

            }

            void pause ();
            void resume ();

            template <phenyl::core::SerializableType T>
            void addComponent (std::string name) {
                runtime().template addComponent<T>(std::move(name));
            };

            template <typename T>
            void addUnserializedComponent () {
                runtime().template addUnserializedComponent<T>();
            }

            core::PhenylRuntime& runtime ();

            const ApplicationProperties& getProperties () const {
                return m_properties;
            }

            double getFixedTimeScale () const {
                return m_fixedTimeScale;
            }

            double getTargetFps () const {
                return m_targetFps;
            }

        protected:
            core::World& world ();

            void setTargetFPS (double fps);
            void setFixedTimeScale (double newTimeScale);

        private:
            ApplicationProperties m_properties;
            core::PhenylRuntime* m_runtime = nullptr;
            double m_targetFrameTime{1.0 / 60};
            double m_targetFps{60};
            double m_fixedTimeScale{1.0};

            virtual void _init () = 0;
            ApplicationBase (ApplicationProperties properties);

            friend class AppPlugin;
            template <PluginType ...Plugins>
            friend class ::phenyl::Application;
        };
    }

    template <PluginType ...Plugins>
    class Application : public engine::ApplicationBase {
    public:
        explicit Application (ApplicationProperties properties) : ApplicationBase{std::move(properties)} {}

        virtual void init () = 0;

    private:
        template <PluginType T, PluginType ...Args>
        static void InitPlugins (core::PhenylRuntime& runtime) {
            runtime.addPlugin<T>();

            if constexpr (sizeof...(Args) > 0) {
                InitPlugins<Args...>();
            }
        }

        void _init() final {
            InitPlugins<Plugins...>(runtime());

            init();
        }
    };

    template <>
    class Application<> : public engine::ApplicationBase {
    public:
        explicit Application (ApplicationProperties properties) : ApplicationBase{std::move(properties)} {}

        virtual void init () = 0;

    private:
        void _init() final {
            init();
        }
    };

    using Application2D = Application<Default2DPlugin>;
    using Application3D = Application<Default3DPlugin>;
}
