
#include "PlayListFileScanner.hpp"

#include <fstream>
#include <optional>

#include "core/ILogger.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/MediaLibrary.hpp"
#include "database/objects/PlayListFile.hpp"

#include "services/scanner/ScanErrors.hpp"

#include "scanners/FileScanOperationBase.hpp"
#include "scanners/Utils.hpp"
#include "scanners/playlist/PlayListParser.hpp"

namespace lms::scanner
{
    namespace
    {
        class PlayListFileScanOperation : public FileScanOperationBase
        {
        public:
            using FileScanOperationBase::FileScanOperationBase;
            ~PlayListFileScanOperation() override = default;
            PlayListFileScanOperation(const PlayListFileScanOperation&) = delete;
            PlayListFileScanOperation& operator=(const PlayListFileScanOperation&) = delete;

        private:
            core::LiteralString getName() const override { return "ScanPlayListFile"; }
            void scan() override;
            OperationResult processResult() override;

            std::optional<PlayList> _parsedPlayList;
        };

        void PlayListFileScanOperation::scan()
        {
            std::ifstream ifs{ getFilePath() };
            if (!ifs)
            {
                const std::error_code ec{ errno, std::generic_category() };

                addError<IOScanError>(getFilePath(), ec);
                return;
            }

            _parsedPlayList = parsePlayList(ifs);
        }

        PlayListFileScanOperation::OperationResult PlayListFileScanOperation::processResult()
        {
            db::Session& dbSession{ getDb().getTLSSession() };
            db::PlayListFile::pointer playList{ db::PlayListFile::find(dbSession, getFilePath()) };

            if (!_parsedPlayList)
            {
                if (playList)
                {
                    playList.remove();

                    LMS_LOG(DBUPDATER, DEBUG, "Removed playlist file " << getFilePath());
                    return OperationResult::Removed;
                }

                return OperationResult::Skipped;
            }

            const bool added{ !playList };
            if (!playList)
                playList = dbSession.create<db::PlayListFile>(getFilePath());

            playList.modify()->setLastWriteTime(getLastWriteTime());
            playList.modify()->setFileSize(getFileSize());
            if (!_parsedPlayList->name.empty())
                playList.modify()->setName(_parsedPlayList->name);
            else
                playList.modify()->setName(getFilePath().stem().string());
            playList.modify()->setFiles(_parsedPlayList->files);

            db::MediaLibrary::pointer mediaLibrary{ db::MediaLibrary::find(dbSession, getMediaLibrary().id) }; // may be null if settings are updated in // => next scan will correct this
            playList.modify()->setDirectory(utils::getOrCreateDirectory(dbSession, getFilePath().parent_path(), mediaLibrary));

            if (added)
            {
                LMS_LOG(DBUPDATER, DEBUG, "Added playlist file " << getFilePath());
                return OperationResult::Added;
            }
            else
            {
                LMS_LOG(DBUPDATER, DEBUG, "Updated playlist file '" << getFilePath());
                return OperationResult::Updated;
            }
        }
    } // namespace

    PlayListFileScanner::PlayListFileScanner(db::IDb& db, ScannerSettings& settings)
        : _db{ db }
        , _settings{ settings }
    {
    }

    core::LiteralString PlayListFileScanner::getName() const
    {
        return "PlayList scanner";
    }

    std::span<const std::filesystem::path> PlayListFileScanner::getSupportedFiles() const
    {
        return {};
    }

    std::span<const std::filesystem::path> PlayListFileScanner::getSupportedExtensions() const
    {
        return getSupportedPlayListFileExtensions();
    }

    bool PlayListFileScanner::needsScan(const FileToScan& file) const
    {
        db::Session& dbSession{ _db.getTLSSession() };
        auto transaction{ _db.getTLSSession().createReadTransaction() };

        const db::PlayListFile::pointer playList{ db::PlayListFile::find(dbSession, file.filePath) };
        return !playList || playList->getLastWriteTime() != file.lastWriteTime;
    }

    std::unique_ptr<IFileScanOperation> PlayListFileScanner::createScanOperation(FileToScan&& fileToScan) const
    {
        return std::make_unique<PlayListFileScanOperation>(std::move(fileToScan), _db, _settings);
    }
} // namespace lms::scanner
