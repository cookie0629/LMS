
#include "Utils.hpp"

#include <system_error>

#include "core/Path.hpp"
#include "database/Session.hpp"
#include "database/objects/Directory.hpp"
#include "database/objects/MediaLibrary.hpp"

namespace lms::scanner::utils
{
    db::Directory::pointer getOrCreateDirectory(db::Session& session, const std::filesystem::path& path, const db::MediaLibrary::pointer& mediaLibrary)
    {
        db::Directory::pointer directory{ db::Directory::find(session, path) };
        if (!directory)
        {
            db::Directory::pointer parentDirectory;
            if (path != mediaLibrary->getPath())
                parentDirectory = getOrCreateDirectory(session, path.parent_path(), mediaLibrary);

            directory = session.create<db::Directory>(path);
            directory.modify()->setParent(parentDirectory);
            directory.modify()->setMediaLibrary(mediaLibrary);
        }
        // Don't update library if it does not match, will be updated elsewhere

        return directory;
    }
} // namespace lms::scanner::utils