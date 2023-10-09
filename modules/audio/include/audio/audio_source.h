#pragma once

#include <cstddef>

#include "common/assets/asset.h"

#include "audio_sample.h"

namespace phenyl::audio {
    class AudioSystem;

    class AudioSource {
    private:
        AudioSystem* audioSystem;
        std::size_t sourceId;
        AudioSource (AudioSystem* audioSystem, std::size_t sourceId) : audioSystem{audioSystem}, sourceId{sourceId} {}
        friend class AudioSystem;
    public:
        AudioSource () : audioSystem{nullptr}, sourceId{0} {}

        explicit operator bool () const {
            return audioSystem && sourceId;
        }

        AudioSource (const AudioSource&) = delete;
        AudioSource (AudioSource&& other) noexcept;

        AudioSource& operator= (const AudioSource&) = delete;
        AudioSource& operator= (AudioSource&& other) noexcept;

        void play (const common::Asset<AudioSample>& sample);
        void play (const AudioSample& sample);

        [[nodiscard]] float getGain () const;
        void setGain (float gain);

        ~AudioSource();
    };
}