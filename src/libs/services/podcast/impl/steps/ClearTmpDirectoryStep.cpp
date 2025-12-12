
#include "ClearTmpDirectoryStep.hpp"

#include "core/ILogger.hpp"

namespace lms::podcast
{
    namespace
    {
        bool clearDirectory(const std::filesystem::path& _rootPath)
        {
            for (const auto& entry : std::filesystem::directory_iterator{ _rootPath })
            {
                std::error_code ec;
                std::filesystem::remove_all(entry, ec);
                if (ec)
                {
                    LMS_LOG(PODCAST, ERROR, "Failed to remove " << entry << ": " << ec.message());
                    return false;
                }
            }

            return true;
        }
    } // namespace

    core::LiteralString ClearTmpDirectoryStep::getName() const
    {
        return "Clear tmp Directory";
    }

    void ClearTmpDirectoryStep::run()
    {
        if (!clearDirectory(getTmpCachePath()))
        {
            LMS_LOG(PODCAST, ERROR, "Failed to delete tmp directory " << getTmpCachePath() << ": aborting refresh");
            onAbort();
            return;
        }

        onDone();
    }

} // namespace lms::podcast