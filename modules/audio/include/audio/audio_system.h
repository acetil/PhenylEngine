#pragma once

#include <memory>

namespace phenyl::audio {
    class AudioSystem {
    private:
    public:
        virtual ~AudioSystem() = default;
    };

    std::unique_ptr<AudioSystem> MakeOpenALSystem ();
}