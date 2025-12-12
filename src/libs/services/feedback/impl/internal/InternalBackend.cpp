
#include "InternalBackend.hpp"

#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/StarredArtist.hpp"
#include "database/objects/StarredRelease.hpp"
#include "database/objects/StarredTrack.hpp"

namespace lms::feedback
{
    namespace details
    {
        template<typename StarredObjType>
        void onStarred(db::Session& session, typename StarredObjType::IdType id)
        {
            auto transaction{ session.createWriteTransaction() };

            if (auto starredObj{ StarredObjType::find(session, id) })
                starredObj.modify()->setSyncState(db::SyncState::Synchronized);
        }

        template<typename StarredObjType>
        void onUnstarred(db::Session& session, typename StarredObjType::IdType id)
        {
            auto transaction{ session.createWriteTransaction() };

            if (auto starredObj{ StarredObjType::find(session, id) })
                starredObj.remove();
        }
    } // namespace details

    InternalBackend::InternalBackend(db::IDb& db)
        : _db{ db }
    {
    }

    void InternalBackend::onStarred(db::StarredArtistId artistId)
    {
        details::onStarred<db::StarredArtist>(_db.getTLSSession(), artistId);
    }

    void InternalBackend::onUnstarred(db::StarredArtistId artistId)
    {
        details::onUnstarred<db::StarredArtist>(_db.getTLSSession(), artistId);
    }

    void InternalBackend::onStarred(db::StarredReleaseId releaseId)
    {
        details::onStarred<db::StarredRelease>(_db.getTLSSession(), releaseId);
    }

    void InternalBackend::onUnstarred(db::StarredReleaseId releaseId)
    {
        details::onUnstarred<db::StarredRelease>(_db.getTLSSession(), releaseId);
    }

    void InternalBackend::onStarred(db::StarredTrackId trackId)
    {
        details::onStarred<db::StarredTrack>(_db.getTLSSession(), trackId);
    }

    void InternalBackend::onUnstarred(db::StarredTrackId trackId)
    {
        details::onUnstarred<db::StarredTrack>(_db.getTLSSession(), trackId);
    }
} // namespace lms::feedback
