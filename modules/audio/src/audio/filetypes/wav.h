#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <optional>

namespace phenyl::audio {
class WAVFile {
public:
    static std::optional<WAVFile> Load (std::istream& file);

    [[nodiscard]] std::uint32_t sampleRate () const;
    [[nodiscard]] std::uint16_t bitDepth () const;
    [[nodiscard]] std::uint16_t numChannels () const;
    [[nodiscard]] std::uint32_t dataSize () const;
    [[nodiscard]] const std::byte* data () const;

private:
    std::uint32_t m_sampleRate;
    std::uint16_t m_bitDepth;
    std::uint16_t m_channels;
    std::uint32_t m_dataSize;
    std::unique_ptr<std::byte[]> m_data;
    WAVFile (std::unique_ptr<std::byte[]> data, std::uint32_t sampleRate, std::uint16_t bitDepth,
        std::uint16_t channels, std::uint32_t dataSize);
};
} // namespace phenyl::audio
