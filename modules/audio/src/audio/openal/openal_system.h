#pragma once

#include "openal_headers.h"

#include "util/fl_vector.h"

#include "audio/audio_system.h"
#include "openal_source.h"
#include "openal_buffer.h"

namespace phenyl::audio {
    class OpenALSystem : public AudioSystem {
    private:
        ALCdevice* device{nullptr};
        ALCcontext* context{nullptr};

        util::FLVector<OpenALSource> sources;
        util::FLVector<OpenALBuffer> buffers;
    public:
        OpenALSystem ();
        ~OpenALSystem() override;

        OpenALSystem (const OpenALSystem&) = delete;
        OpenALSystem (OpenALSystem&& other) noexcept;

        OpenALSystem& operator= (const OpenALSystem&) = delete;
        OpenALSystem& operator= (OpenALSystem&& other) noexcept;

        AudioSource createSource () override;
        AudioSample makeWAVSample (const phenyl::audio::WAVFile &wavFile) override;

        void destroySource (std::size_t id) override;
        void destroySample (std::size_t id) override;

        void playSample (std::size_t sourceId, std::size_t sampleId) override;

        [[nodiscard]] float getSourceGain (std::size_t id) const override;
        void setSourceGain (std::size_t id, float gain) override;
    };
}