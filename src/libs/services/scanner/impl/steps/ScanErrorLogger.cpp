
#include "ScanErrorLogger.hpp"

#include <cassert>

#include "core/ILogger.hpp"

namespace lms::scanner
{
    void ScanErrorLogger::visit([[maybe_unused]] const scanner::ScanError& error)
    {
        // 理论上不应出现“纯 ScanError”而没有具体子类型。
        // Теоретически ScanError без конкретного подтипа возникать не должен.
        assert(false);
    }

    void ScanErrorLogger::visit(const scanner::IOScanError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to open file " << error.path << ": " << error.err.message());
    }

    void ScanErrorLogger::visit(const scanner::AudioFileScanError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to parse audio file " << error.path);
    }

    void ScanErrorLogger::visit(const scanner::EmbeddedImageScanError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to parse image in track file " << error.path << " at index " << error.index << ": " << error.errorMsg);
    }

    void ScanErrorLogger::visit(const scanner::NoAudioTrackFoundError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to parse audio file " << error.path << ": no audio track found");
    }

    void ScanErrorLogger::visit(const scanner::BadAudioDurationError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to parse audio file " << error.path << ": duration is 0");
    }

    void ScanErrorLogger::visit(const scanner::ArtistInfoFileScanError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to read artist info file " << error.path);
    }

    void ScanErrorLogger::visit(const scanner::MissingArtistNameError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to read artist info file " << error.path << ": missing name");
    }

    void ScanErrorLogger::visit(const scanner::ImageFileScanError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to read image file " << error.path << ": " << error.errorMsg);
    }

    void ScanErrorLogger::visit(const scanner::LyricsFileScanError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to read lyrics file " << error.path);
    }

    void ScanErrorLogger::visit(const scanner::PlayListFileScanError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to read playlist file " << error.path);
    }

    void ScanErrorLogger::visit(const scanner::PlayListFilePathMissingError& error)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Track " << error.entry << " not found in playlist " << error.path);
    }

    void ScanErrorLogger::visit(const scanner::PlayListFileAllPathesMissingError& error)
    {
        LMS_LOG(DBUPDATER, ERROR, "Failed to parse playlist " << error.path << ": all entries are missing");
    }
} // namespace lms::scanner