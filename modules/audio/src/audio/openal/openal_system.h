#pragma once

#include "audio/audio_system.h"

#include "openal_headers.h"

namespace phenyl::audio {
    class OpenALSystem : public AudioSystem {
    private:
        ALCdevice* device{nullptr};

    public:
        OpenALSystem ();
        ~OpenALSystem() override;
    };
}