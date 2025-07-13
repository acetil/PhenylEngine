#pragma once

#include "audio_sample.h"
#include "core/assets/asset.h"

#include <cstddef>

namespace phenyl::audio {
class AudioSystem;

class AudioSource {
public:
    AudioSource () : m_audioSystem{nullptr}, m_id{0} {}

    explicit operator bool () const {
        return m_audioSystem && m_id;
    }

    AudioSource (const AudioSource&) = delete;
    AudioSource (AudioSource&& other) noexcept;

    AudioSource& operator= (const AudioSource&) = delete;
    AudioSource& operator= (AudioSource&& other) noexcept;

    void play (const AudioSample& sample);

    [[nodiscard]] float getGain () const;
    void setGain (float gain);

    ~AudioSource ();

private:
    AudioSystem* m_audioSystem;
    std::size_t m_id;

    AudioSource (AudioSystem* audioSystem, std::size_t sourceId) : m_audioSystem{audioSystem}, m_id{sourceId} {}
    friend class AudioSystem;
};
} // namespace phenyl::audio
