#include "ScanStepRemoveOrphanedDbEntries.hpp"

#include <filesystem>
#include <vector>

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Directory.hpp"
#include "database/objects/MediaLibrary.hpp"
#include "database/objects/Track.hpp"

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

            if (candidateGeneric.size() < rootGeneric.size())
            {
                return candidateGeneric == rootGeneric.substr(0, candidateGeneric.size());
            }

            return candidateGeneric == rootGeneric.substr(0, rootGeneric.size() - 1) ||
                   candidateGeneric.rfind(rootGeneric, 0) == 0;
        }
    } // namespace

    ScanStepRemoveOrphanedDbEntries::ScanStepRemoveOrphanedDbEntries(db::IDb& db, const ScannerSettings& settings)
        : ScanStepBase{ db, settings }
    {
    }

    bool ScanStepRemoveOrphanedDbEntries::execute(ScanStats& stats)
    {
        LMS_LOG(SCANNER, INFO, "Starting remove orphaned DB entries step");

        try
        {
            std::filesystem::path mediaLibraryPath;
            if (auto* config = lms::core::Service<lms::core::IConfig>::get())
            {
                mediaLibraryPath = config->getPath("media-library-path", std::filesystem::path{});
            }

            if (mediaLibraryPath.empty())
            {
                LMS_LOG(SCANNER, WARNING, "Media library path not configured, skipping orphan cleanup step");
                return true;
            }

            auto& session = getDb().getTLSSession();

            std::vector<db::Directory::pointer> directories;
            std::vector<db::Track::pointer> tracks;

            {
                auto readTx = session.createReadTransaction();
                db::Directory::find(session, [&directories](const db::Directory::pointer& dir) {
                    directories.push_back(dir);
                });
                db::Track::find(session, [&tracks](const db::Track::pointer& track) {
                    tracks.push_back(track);
                });
                readTx.commit();
            }

            std::size_t removedDirectories{};
            std::size_t removedTracks{};

            {
                auto writeTx = session.createWriteTransaction();
                for (auto& directory : directories)
                {
                    if (!directory)
                    {
                        continue;
                    }

                    const auto dirPath = directory->getAbsolutePath();
                    if (!std::filesystem::exists(dirPath) || !isPathInside(mediaLibraryPath, dirPath))
                    {
                        directory.remove();
                        ++removedDirectories;
                    }
                }
                writeTx.commit();
            }

            {
                auto writeTx = session.createWriteTransaction();
                for (auto& track : tracks)
                {
                    if (!track)
                    {
                        continue;
                    }

                    const auto filePath = track->getAbsoluteFilePath();
                    if (!std::filesystem::exists(filePath))
                    {
                        track.remove();
                        ++removedTracks;
                        continue;
                    }

                    const auto directoryId = track->getDirectoryId();
                    if (directoryId.getValue() == 0)
                    {
                        track.remove();
                        ++removedTracks;
                        continue;
                    }

                    auto directory = db::Directory::find(session, directoryId);
                    if (!directory)
                    {
                        track.remove();
                        ++removedTracks;
                        continue;
                    }
                }
                writeTx.commit();
            }

            if (removedDirectories > 0 || removedTracks > 0)
            {
                stats.deletions += removedDirectories + removedTracks;
            }

            LMS_LOG(SCANNER, INFO, "Remove orphaned entries step completed: "
                << removedDirectories << " directories removed, "
                << removedTracks << " tracks removed");
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, ERROR, "Remove orphaned DB entries step failed: " << e.what());
            return false;
        }

        return true;
    }
} // namespace lms::scanner


