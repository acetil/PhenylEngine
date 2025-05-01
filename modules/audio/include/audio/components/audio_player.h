#pragma once

#include "audio/audio_source.h"
#include "core/serialization/serializer_impl.h"

namespace phenyl::audio {
    class AudioPlayer {
    public:
        AudioPlayer () = default;

        AudioPlayer (const AudioPlayer& other);
        AudioPlayer (AudioPlayer&& other) = default;

        AudioPlayer& operator= (const AudioPlayer& other);
        AudioPlayer& operator= (AudioPlayer&& other) = default;

        void play (const core::Asset<AudioSample>& sample);

        [[nodiscard]] float gain () const;
        void setGain (float gain);

    private:
        AudioSource m_source;
        float m_gain{1.0f};

        PHENYL_SERIALIZABLE_INTRUSIVE(AudioPlayer)
        friend class AudioSystem;
    };
}