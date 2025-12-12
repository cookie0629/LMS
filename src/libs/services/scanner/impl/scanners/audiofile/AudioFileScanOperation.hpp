
#pragma once

#include <vector>

#include "audio/AudioTypes.hpp"
#include "audio/IAudioFileInfo.hpp"
#include "audio/IImageReader.hpp"
#include "image/Types.hpp"

#include "scanners/FileScanOperationBase.hpp"
#include "scanners/FileToScan.hpp"
#include "scanners/IFileScanOperation.hpp"

#include "types/TrackMetadata.hpp"

namespace lms::db
{
    class IDb;
} // namespace lms::db

namespace lms::scanner
{
    class TrackMetadataParser;

    struct ImageInfo
    {
        std::size_t index;
        audio::Image::Type type{ audio::Image::Type::Unknown };
        std::uint64_t hash{};
        std::size_t size{};
        image::ImageProperties properties;
        std::string mimeType;
        std::string description;
    };

    class AudioFileScanOperation : public FileScanOperationBase
    {
    public:
        AudioFileScanOperation(FileToScan&& fileToScan, db::IDb& db, const ScannerSettings& settings, const TrackMetadataParser& metadataParser, const audio::ParserOptions& parserOptions);
        ~AudioFileScanOperation() override;
        AudioFileScanOperation(const AudioFileScanOperation&) = delete;
        AudioFileScanOperation& operator=(const AudioFileScanOperation&) = delete;

    private:
        core::LiteralString getName() const override { return "ScanAudioFile"; }
        void scan() override;
        OperationResult processResult() override;

        const TrackMetadataParser& _metadataParser;
        const audio::ParserOptions& _parserOptions;

        struct AudioFileInfo
        {
            audio::AudioProperties audioProperties;
            Track track;
            std::vector<ImageInfo> images;
        };
        std::optional<AudioFileInfo> _file;
    };
} // namespace lms::scanner