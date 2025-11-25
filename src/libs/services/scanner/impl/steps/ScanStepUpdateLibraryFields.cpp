#include "ScanStepUpdateLibraryFields.hpp"

#include <filesystem>
#include <vector>

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Path.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Directory.hpp"
#include "database/objects/MediaLibrary.hpp"

namespace lms::scanner
{
    namespace
    {
        bool isPathInside(const std::filesystem::path& root, const std::filesystem::path& candidate)
        {
            if (root.empty())
            {
                return false;
            }

            auto rootGeneric = root.generic_string();
            auto candidateGeneric = candidate.generic_string();

            if (rootGeneric.empty() || candidateGeneric.empty())
            {
                return false;
            }

            if (rootGeneric.back() != '/')
            {
                rootGeneric.push_back('/');
            }

            return candidateGeneric == rootGeneric.substr(0, rootGeneric.size() - 1) ||
                   candidateGeneric.rfind(rootGeneric, 0) == 0;
        }
    } // namespace

    ScanStepUpdateLibraryFields::ScanStepUpdateLibraryFields(db::IDb& db, const ScannerSettings& settings)
        : ScanStepBase{ db, settings }
    {
    }

    bool ScanStepUpdateLibraryFields::execute(ScanStats& stats)
    {
        LMS_LOG(SCANNER, INFO, "Starting update library fields step");

        try
        {
            std::filesystem::path mediaLibraryPath;
            if (auto* config = lms::core::Service<lms::core::IConfig>::get())
            {
                mediaLibraryPath = config->getPath("media-library-path", std::filesystem::path{});
            }

            if (mediaLibraryPath.empty())
            {
                LMS_LOG(SCANNER, WARNING, "Media library path not configured, skipping update library fields step");
                return true;
            }

            auto& session = getDb().getTLSSession();

            db::MediaLibrary::pointer mediaLibrary;
            {
                auto readTx = session.createReadTransaction();
                mediaLibrary = db::MediaLibrary::find(session, mediaLibraryPath);
                readTx.commit();
            }

            if (!mediaLibrary)
            {
                auto writeTx = session.createWriteTransaction();
                mediaLibrary = db::MediaLibrary::getOrCreate(
                    session,
                    core::pathUtils::getFilename(mediaLibraryPath),
                    mediaLibraryPath);
                writeTx.commit();
            }

            std::vector<db::Directory::pointer> directories;
            {
                auto readTx = session.createReadTransaction();
                db::Directory::find(session, [&directories](const db::Directory::pointer& dir) {
                    directories.push_back(dir);
                });
                readTx.commit();
            }

            std::size_t updatedCount = 0;

            auto writeTx = session.createWriteTransaction();
            for (auto& directory : directories)
            {
                if (!directory)
                {
                    continue;
                }

                const auto dirPath = directory->getAbsolutePath();
                if (!isPathInside(mediaLibraryPath, dirPath))
                {
                    continue;
                }

                const auto currentLibraryId = directory->getMediaLibraryId();
                if (currentLibraryId == mediaLibrary->getId())
                {
                    continue;
                }

                directory.modify()->setMediaLibrary(mediaLibrary);
                ++updatedCount;
            }
            writeTx.commit();

            if (updatedCount > 0)
            {
                stats.updates += updatedCount;
            }

            LMS_LOG(SCANNER, INFO, "Update library fields step completed: " << updatedCount << " directories updated");
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, ERROR, "Update library fields step failed: " << e.what());
            return false;
        }

        return true;
    }
} // namespace lms::scanner


