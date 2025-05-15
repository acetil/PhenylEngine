#include "wav.h"

#include "audio/detail/loggers.h"
#include "util/endian.h"
#include "util/istream_help.h"

using namespace phenyl;

struct WAVHeader {
    std::uint32_t fileSize;
    std::uint16_t formatType;
    std::uint16_t channels;
    std::uint32_t sampleRate;
    std::uint32_t byteRate;
    std::uint16_t blockAlign;
    std::uint16_t sampleBits;
    std::uint32_t dataSize;
};

static constexpr std::uint32_t MAGIC_TAG = 0x46464952; // "RIFF" 52494646
static constexpr std::uint32_t WAV_TAG = 0x45564157;   // "WAVE"
static constexpr std::uint32_t FMT_TAG = 0x20746d66;   // "fmt " 666d7420
static constexpr std::uint32_t DATA_TAG = 0x61746164;  // "data" 64617461

static constexpr std::uint32_t HEADER_SIZE = 44 - 8;
static constexpr std::uint32_t FORMAT_SIZE = 16;
static constexpr std::uint16_t PCM_TYPE = 1;

static Logger LOGGER{"WAV_FILE", audio::detail::AUDIO_LOGGER};

static std::optional<WAVHeader> readHeader (std::istream& file);

audio::WAVFile::WAVFile (std::unique_ptr<std::byte[]> data, std::uint32_t sampleRate, std::uint16_t bitDepth,
    std::uint16_t channels, std::uint32_t dataSize) :
    m_data{std::move(data)},
    m_sampleRate{sampleRate},
    m_bitDepth{bitDepth},
    m_channels{channels},
    m_dataSize{dataSize} {}

std::uint32_t audio::WAVFile::sampleRate () const {
    return m_sampleRate;
}

std::uint16_t audio::WAVFile::bitDepth () const {
    return m_bitDepth;
}

std::uint16_t audio::WAVFile::numChannels () const {
    return m_channels;
}

std::uint32_t audio::WAVFile::dataSize () const {
    return m_dataSize;
}

const std::byte* audio::WAVFile::data () const {
    return m_data.get();
}

std::optional<audio::WAVFile> audio::WAVFile::Load (std::istream& file) {
    PHENYL_TRACE(LOGGER, "Loading WAV file");
    auto headerOpt = readHeader(file);
    if (!headerOpt) {
        return std::nullopt;
    }

    auto header = *headerOpt;
    auto data = std::make_unique<std::byte[]>(header.dataSize);

    if (!file.read((char*) data.get(), header.dataSize)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV file data!");
        return std::nullopt;
    }

    return WAVFile{std::move(data), header.sampleRate, header.sampleBits, header.channels, header.dataSize};
}

static std::optional<WAVHeader> readHeader (std::istream& file) {
    WAVHeader header{};
    std::uint32_t magic;
    if (!util::BinaryReadLittle(file, magic)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV RIFF file tag!");
        return std::nullopt;
    } else if (magic != MAGIC_TAG) {
        PHENYL_LOGE(LOGGER, "Invalid RIFF file tag: {} (expected {})", magic, MAGIC_TAG);
        return std::nullopt;
    }

    if (!util::BinaryReadLittle(file, header.fileSize)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV file size!");
        return std::nullopt;
    } else if (header.fileSize < HEADER_SIZE) {
        PHENYL_LOGE(LOGGER, "Invalid WAV file size: {}", header.fileSize);
        return std::nullopt;
    }

    std::uint32_t wavTag;
    if (!util::BinaryReadLittle(file, wavTag)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV format tag!");
        return std::nullopt;
    } else if (wavTag != WAV_TAG) {
        PHENYL_LOGE(LOGGER, "Invalid WAV format tag: {} (expected {})", wavTag, WAV_TAG);
        return std::nullopt;
    }

    std::uint32_t fmtTag;
    if (!util::BinaryReadLittle(file, fmtTag)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV file fmt tag!");
        return std::nullopt;
    } else if (fmtTag != FMT_TAG) {
        PHENYL_LOGE(LOGGER, "Invalid WAV format tag: {} (expected {})", fmtTag, FMT_TAG);
        return std::nullopt;
    }

    std::uint32_t fmtSize;
    if (!util::BinaryReadLittle(file, fmtSize)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV fmt size!");
        return std::nullopt;
    } else if (fmtSize != FORMAT_SIZE) {
        PHENYL_LOGE(LOGGER, "Invalid WAV fmt size: {} (expected {})", fmtSize, FORMAT_SIZE);
        return std::nullopt;
    }

    if (!util::BinaryReadLittle(file, header.formatType)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV fmt type!");
        return std::nullopt;
    } else if (header.formatType != PCM_TYPE) {
        PHENYL_LOGE(LOGGER, "Unsupported WAV format type: {}", header.formatType);
        return std::nullopt;
    }

    if (!util::BinaryReadLittle(file, header.channels)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV channels!");
        return std::nullopt;
    }

    if (!util::BinaryReadLittle(file, header.sampleRate)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV sample rate!");
        return std::nullopt;
    }

    if (!util::BinaryReadLittle(file, header.byteRate)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV byte rate!");
        return std::nullopt;
    }

    if (!util::BinaryReadLittle(file, header.blockAlign)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV block align!");
        return std::nullopt;
    }

    if (!util::BinaryReadLittle(file, header.sampleBits)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV sample bits!");
        return std::nullopt;
    }

    if (header.byteRate != header.sampleRate * header.channels * header.sampleBits / 8) {
        PHENYL_LOGE(LOGGER, "Invalid byte rate {} (sample rate: {}, channels: {}, sample bits: {})", header.byteRate,
            header.sampleRate, header.channels, header.sampleBits);
        return std::nullopt;
    }

    if (header.blockAlign != header.channels * header.sampleBits / 8) {
        PHENYL_LOGE(LOGGER, "Invalid block align {} (channels: {}, sample bits: {})", header.blockAlign,
            header.channels, header.sampleBits);
        return std::nullopt;
    }

    std::uint32_t dataTag;
    if (!util::BinaryReadLittle(file, dataTag)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV data tag!");
        return std::nullopt;
    } else if (dataTag != DATA_TAG) {
        PHENYL_LOGE(LOGGER, "Invalid WAV data tag: {} (expected {})", dataTag, DATA_TAG);
        return std::nullopt;
    }

    if (!util::BinaryReadLittle(file, header.dataSize)) {
        PHENYL_LOGE(LOGGER, "Failed to read WAV data size!");
        return std::nullopt;
    } else if (header.dataSize != header.fileSize - HEADER_SIZE) {
        PHENYL_LOGE(LOGGER, "Invalid WAV data size: {} (file size: {}, header size: {})", header.dataSize,
            header.fileSize, HEADER_SIZE);
        return std::nullopt;
    }

    PHENYL_TRACE(LOGGER,
        "Read WAV header: fileSize={}, formatType={}, channels={}, sampleRate={}, byteRate={}, "
        "blockAlign={}, "
        "sampleBits={}, dataSize={}",
        header.fileSize, header.formatType, header.channels, header.sampleRate, header.byteRate, header.blockAlign,
        header.sampleBits, header.dataSize);

    return header;
}
