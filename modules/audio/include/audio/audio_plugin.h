#pragma once

#include "core/plugin.h"

namespace phenyl::audio {
    class AudioSystem;

    class AudioPlugin : public core::IPlugin {
    private:
        std::unique_ptr<AudioSystem> audioSystem;
    public:
        AudioPlugin ();
        ~AudioPlugin() override;

        std::string_view getName() const noexcept override;

        void init (core::PhenylRuntime& runtime) override;
    };
}