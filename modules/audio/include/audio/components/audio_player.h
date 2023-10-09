#pragma once

#include "audio/audio_source.h"

namespace phenyl::audio {
    class AudioPlayer {
    private:
        AudioSource source;
        friend class AudioSystem;
    public:
        AudioPlayer () = default;

        AudioPlayer (const AudioPlayer& other);
        AudioPlayer (AudioPlayer&& other) = default;

        AudioPlayer& operator= (const AudioPlayer& other);
        AudioPlayer& operator= (AudioPlayer&& other) = default;

        void play (const common::Asset<AudioSample>& sample);
    };
}