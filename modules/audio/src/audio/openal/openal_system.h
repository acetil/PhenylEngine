#pragma once

#include "audio/audio_backend.h"
#include "audio/audio_system.h"
#include "openal_buffer.h"
#include "openal_headers.h"
#include "openal_source.h"
#include "util/fl_vector.h"

namespace phenyl::audio {
class OpenALSystem : public AudioBackend {
public:
    OpenALSystem ();
    ~OpenALSystem () override;

    OpenALSystem (const OpenALSystem&) = delete;
    OpenALSystem (OpenALSystem&& other) noexcept;

    OpenALSystem& operator= (const OpenALSystem&) = delete;
    OpenALSystem& operator= (OpenALSystem&& other) noexcept;

    std::size_t makeSource () override;
    std::size_t makeWAVSample (const WAVFile& wavFile) override;

    void destroySource (std::size_t id) override;
    void destroySample (std::size_t id) override;

    void playSample (std::size_t sourceId, std::size_t sampleId) override;

    [[nodiscard]] float getSampleDuration (std::size_t id) const override;

    [[nodiscard]] float getSourceGain (std::size_t id) const override;
    void setSourceGain (std::size_t id, float gain) override;
    void stopSource (std::size_t id) override;
    bool isSourceStopped (std::size_t id) override;

private:
    ALCdevice* m_device{nullptr};
    ALCcontext* m_context{nullptr};

    util::FLVector<OpenALSource> m_sources;
    util::FLVector<OpenALBuffer> m_buffers;
};
} // namespace phenyl::audio
