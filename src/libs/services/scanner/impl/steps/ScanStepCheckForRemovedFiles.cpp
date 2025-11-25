#include "ScanStepCheckForRemovedFiles.hpp"

#include <filesystem>

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/MediaLibrary.hpp"
#include "database/objects/Track.hpp"
#include "services/scanner/ScanErrors.hpp"

namespace lms::scanner
{
    ScanStepCheckForRemovedFiles::ScanStepCheckForRemovedFiles(db::IDb& db, const ScannerSettings& settings)
        : ScanStepBase{ db, settings }
    {
    }

    bool ScanStepCheckForRemovedFiles::execute(ScanStats& stats)
    {
        LMS_LOG(SCANNER, INFO, "Starting check for removed files step");

        try
        {
            // 获取媒体库路径
            std::filesystem::path mediaLibraryPath;
            if (auto* config = lms::core::Service<lms::core::IConfig>::get())
            {
                mediaLibraryPath = config->getPath("media-library-path", std::filesystem::path{});
            }

            if (mediaLibraryPath.empty())
            {
                LMS_LOG(SCANNER, WARNING, "Media library path not configured, skipping removed files check");
                return true; // 不是错误，只是未配置
            }

            auto& session = getDb().getTLSSession();

            // 获取媒体库对象
            db::MediaLibrary::pointer mediaLibrary;
            {
                auto readTx = session.createReadTransaction();
                mediaLibrary = db::MediaLibrary::find(session, mediaLibraryPath);
                readTx.commit();
            }

            if (!mediaLibrary)
            {
                LMS_LOG(SCANNER, WARNING, "Media library not found, skipping removed files check");
                return true;
            }

            // 获取所有 Track（简化版：遍历所有 Track）
            // 完整版本应该只检查属于该媒体库的 Track
            std::vector<db::Track::pointer> tracksToCheck;
            {
                auto readTx = session.createReadTransaction();
                db::Track::find(session, [&tracksToCheck](const db::Track::pointer& track) {
                    tracksToCheck.push_back(track);
                });
                readTx.commit();
            }

            LMS_LOG(SCANNER, INFO, "Checking " << tracksToCheck.size() << " tracks for removed files");

            auto writeTx = session.createWriteTransaction();
            std::size_t removedCount = 0;

            for (auto& track : tracksToCheck)
            {
                const auto filePath = track->getAbsoluteFilePath();

                // 检查文件是否存在
                if (!std::filesystem::exists(filePath))
                {
                    LMS_LOG(SCANNER, DEBUG, "Removing track (file not found): " << filePath);
                    track.remove();
                    ++removedCount;
                    ++stats.deletions;
                }
            }

            writeTx.commit();

            LMS_LOG(SCANNER, INFO, "Check for removed files completed: " << removedCount << " tracks removed");
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, ERROR, "Check for removed files step failed: " << e.what());
            return false;
        }

        return true;
    }
} // namespace lms::scanner

