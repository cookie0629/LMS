
#include "ScanStepUpdateLibraryFields.hpp"

#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/Directory.hpp"
#include "database/objects/MediaLibrary.hpp"

#include "MediaLibraryInfo.hpp"
#include "ScanContext.hpp"
#include "ScannerSettings.hpp"

namespace lms::scanner
{
    bool ScanStepUpdateLibraryFields::needProcess([[maybe_unused]] const ScanContext& context) const
    {
        // Fast enough when nothing to do
        return true;
    }

    void ScanStepUpdateLibraryFields::process(ScanContext& context)
    {
        processDirectories(context);
    }

    void ScanStepUpdateLibraryFields::processDirectories(ScanContext& context)
    {
        for (const MediaLibraryInfo& mediaLibrary : _settings.mediaLibraries)
        {
            if (_abortScan)
                break;

            processDirectory(context, mediaLibrary);
        }
    }

    void ScanStepUpdateLibraryFields::processDirectory(ScanContext& context, const MediaLibraryInfo& mediaLibrary)
    {
        db::Session& session{ _db.getTLSSession() };

        constexpr std::size_t batchSize = 100;

        db::RangeResults<db::DirectoryId> entries;
        while (!_abortScan)
        {
            {
                auto transaction{ session.createReadTransaction() };

                entries = db::Directory::findMismatchedLibrary(session, db::Range{ 0, batchSize }, mediaLibrary.rootDirectory, mediaLibrary.id);
            };

            if (entries.results.empty())
                break;

            {
                auto transaction{ session.createWriteTransaction() };

                db::MediaLibrary::pointer library{ db::MediaLibrary::find(session, mediaLibrary.id) };
                if (!library) // may be legit
                    break;

                for (const db::DirectoryId directoryId : entries.results)
                {
                    if (_abortScan)
                        break;

                    db::Directory::pointer directory{ db::Directory::find(session, directoryId) };
                    directory.modify()->setMediaLibrary(library);
                }
            }

            context.currentStepStats.processedElems += entries.results.size();
            _progressCallback(context.currentStepStats);
        }
    }
} // namespace lms::scanner
