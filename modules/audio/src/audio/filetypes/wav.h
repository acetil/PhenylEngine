#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>

#include "util/optional.h"

namespace phenyl::audio {
    class WAVFile {
    private:
        std::uint32_t sampleRate;
        std::uint16_t bitDepth;
        std::uint16_t channels;
        std::uint32_t dataSize;
        std::unique_ptr<std::byte[]> data;
        WAVFile (std::unique_ptr<std::byte[]> data, std::uint32_t sampleRate, std::uint16_t bitDepth, std::uint16_t channels, std::uint32_t dataSize);
    public:
        static util::Optional<WAVFile> Load (std::istream& file);

        [[nodiscard]] std::uint32_t getSampleRate () const;
        [[nodiscard]] std::uint16_t getBitDepth () const;
        [[nodiscard]] std::uint16_t getNumChannels () const;
        [[nodiscard]] std::uint32_t getDataSize () const;
        [[nodiscard]] const std::byte* getData () const;
    };
}