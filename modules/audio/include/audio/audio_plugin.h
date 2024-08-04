#pragma once

#include "runtime/plugin.h"

namespace phenyl::audio {
    class AudioSystem;

    class AudioPlugin : public runtime::IPlugin {
    private:
        std::unique_ptr<AudioSystem> audioSystem;
    public:
        AudioPlugin ();
        ~AudioPlugin() override;

        std::string_view getName() const noexcept override;

        void init (runtime::PhenylRuntime& runtime) override;
    };
}