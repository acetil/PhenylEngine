#pragma once

#include <cstddef>

namespace phenyl::audio {
    class AudioSystem;
    class AudioSample {
    private:
        AudioSystem* audioSystem{nullptr};
        std::size_t sampleId{0};

        friend AudioSystem;
        friend class AudioSource;
        AudioSample (AudioSystem* audioSystem, std::size_t sampleId) : audioSystem{audioSystem}, sampleId{sampleId} {}
    public:
        AudioSample () = default;

        AudioSample (const AudioSample&) = delete;
        AudioSample (AudioSample&& other) noexcept;

        AudioSample& operator= (const AudioSample&) = delete;
        AudioSample& operator= (AudioSample&& other) noexcept;

        explicit operator bool () const {
            return audioSystem && sampleId;
        }

        [[nodiscard]] float duration () const;

        ~AudioSample();
    };
}