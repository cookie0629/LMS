
#pragma once

#include "services/scanner/ScanErrors.hpp"

namespace lms::scanner
{
    class ScanErrorLogger : public scanner::ScanErrorVisitor
    {
    private:
        void visit(const scanner::ScanError&) override;
        void visit(const scanner::IOScanError& error) override;
        void visit(const scanner::AudioFileScanError& error) override;
        void visit(const scanner::EmbeddedImageScanError& error) override;
        void visit(const scanner::NoAudioTrackFoundError& error) override;
        void visit(const scanner::BadAudioDurationError& error) override;
        void visit(const scanner::ArtistInfoFileScanError& error) override;
        void visit(const scanner::MissingArtistNameError& error) override;
        void visit(const scanner::ImageFileScanError& error) override;
        void visit(const scanner::LyricsFileScanError& error) override;
        void visit(const scanner::PlayListFileScanError& error) override;
        void visit(const scanner::PlayListFilePathMissingError& error) override;
        void visit(const scanner::PlayListFileAllPathesMissingError& error) override;
    };
} // namespace lms::scanner