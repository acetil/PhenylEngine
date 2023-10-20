#pragma once

#include <cstddef>

namespace phenyl::audio {
    class WAVFile;

    class AudioBackend {
    public:
        virtual ~AudioBackend() = default;
        virtual std::size_t makeSource () = 0;

        [[nodiscard]] virtual float getSourceGain (std::size_t id) const = 0;
        virtual void setSourceGain (std::size_t id, float gain) = 0;

        virtual void destroySource (std::size_t id) = 0;

        virtual std::size_t makeWAVSample (const WAVFile& wavFile) = 0;

        [[nodiscard]] virtual float getSampleDuration (std::size_t id) const = 0;

        virtual void playSample (std::size_t sourceId, std::size_t sampleId) = 0;
        virtual void stopSource (std::size_t sourceId) = 0;
        virtual bool isSourceStopped (std::size_t id) = 0;

        virtual void destroySample (std::size_t id) = 0;
    };
}