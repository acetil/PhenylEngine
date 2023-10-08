#include "wav.h"
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
static constexpr std::uint32_t WAV_TAG = 0x45564157; // "WAVE"
static constexpr std::uint32_t FMT_TAG = 0x20746d66;   // "fmt " 666d7420
static constexpr std::uint32_t DATA_TAG = 0x61746164; // "data" 64617461

static constexpr std::uint32_t HEADER_SIZE = 44 - 8;
static constexpr std::uint32_t FORMAT_SIZE = 16;
static constexpr std::uint16_t PCM_TYPE = 1;


static util::Optional<WAVHeader> readHeader (std::istream& file);

audio::WAVFile::WAVFile (std::unique_ptr<std::byte[]> data, std::uint32_t sampleRate, std::uint16_t bitDepth, std::uint16_t channels, std::uint32_t dataSize) :
        data{std::move(data)}, sampleRate{sampleRate}, bitDepth{bitDepth}, channels{channels}, dataSize{dataSize} {}

std::uint32_t audio::WAVFile::getSampleRate () const {
    return sampleRate;
}

std::uint16_t audio::WAVFile::getBitDepth () const {
    return bitDepth;
}

std::uint16_t audio::WAVFile::getNumChannels () const {
    return channels;
}

std::uint32_t audio::WAVFile::getDataSize () const {
    return dataSize;
}

const std::byte* audio::WAVFile::getData () const {
    return data.get();
}

util::Optional<audio::WAVFile> audio::WAVFile::Load (std::istream& file) {
    auto headerOpt = readHeader(file);
    if (!headerOpt) {
        return util::NullOpt;
    }

    auto header = headerOpt.getUnsafe();
    auto data = std::make_unique<std::byte[]>(header.dataSize);

    if (!file.read((char*)data.get(), header.dataSize)) {
        logging::log(LEVEL_ERROR, "Failed to read WAV file data!");
        return util::NullOpt;
    }

    return WAVFile{std::move(data), header.sampleRate, header.sampleBits, header.channels, header.dataSize};
}

static util::Optional<WAVHeader> readHeader (std::istream& file) {
    WAVHeader header{};
    std::uint32_t magic;
    if (!util::BinaryReadLittle(file, magic)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV RIFF file tag!");
        return util::NullOpt;
    } else if (magic != MAGIC_TAG) {
        audio::logging::log(LEVEL_ERROR, "Invalid RIFF file tag: {} (expected {})", magic, MAGIC_TAG);
        return util::NullOpt;
    }

    if (!util::BinaryReadLittle(file, header.fileSize)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV file size!");
        return util::NullOpt;
    } else if (header.fileSize < HEADER_SIZE) {
        audio::logging::log(LEVEL_ERROR, "Invalid WAV file size: {}", header.fileSize);
        return util::NullOpt;
    }

    std::uint32_t wavTag;
    if (!util::BinaryReadLittle(file, wavTag)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV format tag!");
        return util::NullOpt;
    } else if (wavTag != WAV_TAG) {
        audio::logging::log(LEVEL_ERROR, "Invalid WAV format tag: {} (expected {})", wavTag, WAV_TAG);
        return util::NullOpt;
    }


    std::uint32_t fmtTag;
    if (!util::BinaryReadLittle(file, fmtTag)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV file fmt tag!");
        return util::NullOpt;
    } else if (fmtTag != FMT_TAG) {
        audio::logging::log(LEVEL_ERROR, "Invalid WAV format tag: {} (expected {})", fmtTag, FMT_TAG);
        return util::NullOpt;
    }

    std::uint32_t fmtSize;
    if (!util::BinaryReadLittle(file, fmtSize)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV fmt size!");
        return util::NullOpt;
    } else if (fmtSize != FORMAT_SIZE) {
        audio::logging::log(LEVEL_ERROR, "Invalid WAV fmt size: {} (expected {})", fmtSize, FORMAT_SIZE);
        return util::NullOpt;
    }

    if (!util::BinaryReadLittle(file, header.formatType)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV fmt type!");
        return util::NullOpt;
    } else if (header.formatType != PCM_TYPE) {
        audio::logging::log(LEVEL_ERROR, "Unsupported WAV format type: {}", header.formatType);
        return util::NullOpt;
    }

    if (!util::BinaryReadLittle(file, header.channels)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV channels!");
        return util::NullOpt;
    }

    if (!util::BinaryReadLittle(file, header.sampleRate)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV sample rate!");
        return util::NullOpt;
    }

    if (!util::BinaryReadLittle(file, header.byteRate)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV byte rate!");
        return util::NullOpt;
    }

    if (!util::BinaryReadLittle(file, header.blockAlign)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV block align!");
        return util::NullOpt;
    }

    if (!util::BinaryReadLittle(file, header.sampleBits)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV sample bits!");
        return util::NullOpt;
    }

    if (header.byteRate != header.sampleRate * header.channels * header.sampleBits / 8) {
        audio::logging::log(LEVEL_ERROR, "Invalid byte rate {} (sample rate: {}, channels: {}, sample bits: {})", header.byteRate, header.sampleRate, header.channels, header.sampleBits);
        return util::NullOpt;
    }

    if (header.blockAlign != header.channels * header.sampleBits / 8) {
        audio::logging::log(LEVEL_ERROR, "Invalid block align {} (channels: {}, sample bits: {})", header.blockAlign, header.channels, header.sampleBits);
        return util::NullOpt;
    }

    std::uint32_t dataTag;
    if (!util::BinaryReadLittle(file, dataTag)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV data tag!");
        return util::NullOpt;
    } else if (dataTag != DATA_TAG) {
        audio::logging::log(LEVEL_ERROR, "Invalid WAV data tag: {} (expected {})", dataTag, DATA_TAG);
        return util::NullOpt;
    }

    if (!util::BinaryReadLittle(file, header.dataSize)) {
        audio::logging::log(LEVEL_ERROR, "Failed to read WAV data size!");
        return util::NullOpt;
    } else if (header.dataSize != header.fileSize - HEADER_SIZE) {
        audio::logging::log(LEVEL_ERROR, "Invalid WAV data size: {} (file size: {}, header size: {})", header.dataSize, header.fileSize, HEADER_SIZE);
        return util::NullOpt;
    }

    return header;
}


