#include "LyricsFileScanner.hpp"

#include <fstream>
#include <optional>

#include "core/ILogger.hpp"

#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/MediaLibrary.hpp"
#include "database/objects/TrackLyrics.hpp"
#include "services/scanner/ScanErrors.hpp"

#include "ScannerSettings.hpp"
#include "scanners/FileScanOperationBase.hpp"
#include "scanners/Utils.hpp"
#include "LyricsParser.hpp"
#include "types/Lyrics.hpp"

namespace lms::scanner
{
    namespace
    {
        class LyricsFileScanOperation : public FileScanOperationBase
        {
        public:
            using FileScanOperationBase::FileScanOperationBase;

        private:
            std::string_view getName() const override { return "ScanLyricsFile"; }
            void scan() override;
            OperationResult processResult() override;

            std::optional<Lyrics> _parsedLyrics;
        };

        void LyricsFileScanOperation::scan()
        {
            std::ifstream ifs{ getFilePath() };
            if (!ifs)
            {
                const std::error_code ec{ errno, std::generic_category() };

                addError<FileScanError>(getFilePath(), "Unable to open lyrics file: " + ec.message());
                return;
            }

            _parsedLyrics = parseLyrics(ifs);
        }

        LyricsFileScanOperation::OperationResult LyricsFileScanOperation::processResult()
        {
            db::Session& dbSession{ getDb().getTLSSession() };
            auto transaction = dbSession.createWriteTransaction();

            db::TrackLyrics::pointer trackLyrics{ db::TrackLyrics::find(dbSession, getFilePath()) };

            if (!_parsedLyrics)
            {
                if (trackLyrics)
                {
                    trackLyrics.remove();

                    LMS_LOG(DBUPDATER, DEBUG, "Removed lyrics file " << getFilePath());
                    return OperationResult::Removed;
                }

                return OperationResult::Skipped;
            }

            const bool added{ !trackLyrics };
            if (!trackLyrics)
            {
                trackLyrics = dbSession.create<db::TrackLyrics>();
                trackLyrics.modify()->setAbsoluteFilePath(getFilePath());
            }

            trackLyrics.modify()->setLastWriteTime(getLastWriteTime());
            trackLyrics.modify()->setFileSize(getFileSize());
            trackLyrics.modify()->setLanguage(!_parsedLyrics->language.empty() ? _parsedLyrics->language : "xxx");
            trackLyrics.modify()->setOffset(_parsedLyrics->offset);
            trackLyrics.modify()->setDisplayTitle(_parsedLyrics->displayTitle);
            trackLyrics.modify()->setDisplayArtist(_parsedLyrics->displayArtist);
            if (!_parsedLyrics->synchronizedLines.empty())
                trackLyrics.modify()->setSynchronizedLines(_parsedLyrics->synchronizedLines);
            else
                trackLyrics.modify()->setUnsynchronizedLines(_parsedLyrics->unsynchronizedLines);

            db::MediaLibrary::pointer mediaLibrary{ db::MediaLibrary::find(dbSession, getMediaLibrary().id) };
            if (mediaLibrary)
            {
                db::Directory::pointer directory{ db::Directory::getOrCreate(dbSession, getFilePath().parent_path(), mediaLibrary) };
                trackLyrics.modify()->setDirectory(ObjectPtr<db::Directory>(directory));
            }

            if (added)
            {
                LMS_LOG(DBUPDATER, DEBUG, "Added external lyrics " << getFilePath());
                return OperationResult::Added;
            }

            LMS_LOG(DBUPDATER, DEBUG, "Updated external lyrics " << getFilePath());
            return OperationResult::Updated;
        }
    } // namespace

    LyricsFileScanner::LyricsFileScanner(db::IDb& db, const ScannerSettings& settings)
        : _db{ db }
        , _settings{ settings }
    {
    }

    std::string_view LyricsFileScanner::getName() const
    {
        return "Lyrics scanner";
    }

    std::span<const std::filesystem::path> LyricsFileScanner::getSupportedFiles() const
    {
        return {};
    }

    std::span<const std::filesystem::path> LyricsFileScanner::getSupportedExtensions() const
    {
        return getSupportedLyricsFileExtensions();
    }

    bool LyricsFileScanner::needsScan(const FileToScan& file) const
    {
        db::Session& dbSession{ _db.getTLSSession() };
        db::TrackLyrics::pointer lyrics{ db::TrackLyrics::find(dbSession, file.filePath) };
        return !lyrics || lyrics->getLastWriteTime() != file.lastWriteTime;
    }

    std::unique_ptr<IFileScanOperation> LyricsFileScanner::createScanOperation(FileToScan&& fileToScan) const
    {
        return std::make_unique<LyricsFileScanOperation>(std::move(fileToScan), _db, _settings);
    }
} // namespace lms::scanner


