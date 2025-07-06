#pragma once

#include "core/assets/asset.h"

#include <cstddef>

namespace phenyl::audio {
class AudioSystem;

class AudioSample : public core::AssetType2<AudioSample> {
public:
    AudioSample () = default;

    AudioSample (const AudioSample&) = delete;
    AudioSample (AudioSample&& other) noexcept;

    AudioSample& operator= (const AudioSample&) = delete;
    AudioSample& operator= (AudioSample&& other) noexcept;

    explicit operator bool () const {
        return m_audioSystem && m_id;
    }

    [[nodiscard]] float duration () const;

    ~AudioSample () override;

private:
    AudioSystem* m_audioSystem{nullptr};
    std::size_t m_id{0};

    friend AudioSystem;
    friend class AudioSource;

    AudioSample (AudioSystem* audioSystem, std::size_t sampleId) : m_audioSystem{audioSystem}, m_id{sampleId} {}
};
} // namespace phenyl::audio
