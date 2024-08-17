#pragma once

#include "audio/audio_source.h"
#include "common/serializer_intrusive.h"

namespace phenyl::audio {
    class AudioPlayer {
    private:
        AudioSource source;
        float sourceGain{1.0f};

        PHENYL_SERIALIZE_INTRUSIVE(AudioPlayer)
        friend class AudioSystem;
    public:
        AudioPlayer () = default;

        AudioPlayer (const AudioPlayer& other);
        AudioPlayer (AudioPlayer&& other) = default;

        AudioPlayer& operator= (const AudioPlayer& other);
        AudioPlayer& operator= (AudioPlayer&& other) = default;

        void play (const common::Asset<AudioSample>& sample);

        [[nodiscard]] float gain () const;
        void setGain (float gain);
    };
}