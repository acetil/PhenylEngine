#pragma once

#include <cstddef>

namespace phenyl::audio {
    class AudioSystem;
    class AudioSample {
    private:
        AudioSystem* audioSystem{nullptr};
        std::size_t sampleId{0};

        AudioSample (AudioSystem* audioSystem, std::size_t sampleId) : audioSystem{audioSystem}, sampleId{sampleId} {}
        friend class AudioSystem;
        friend class AudioSource;
    public:
        AudioSample () = default;

        AudioSample (const AudioSample&) = delete;
        AudioSample (AudioSample&& other) noexcept;

        AudioSample& operator= (const AudioSample&) = delete;
        AudioSample& operator= (AudioSample&& other) noexcept;

        explicit operator bool () const {
            return audioSystem && sampleId;
        }

        ~AudioSample();
    };
}