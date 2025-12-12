
#include "InternalBackend.hpp"

#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/Listen.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/User.hpp"

namespace lms::scrobbling
{
    InternalBackend::InternalBackend(db::IDb& db)
        : _db{ db }
    {
    }

    void InternalBackend::listenStarted(const Listen&)
    {
        // nothing to do
    }

    void InternalBackend::listenFinished(const Listen& listen, std::optional<std::chrono::seconds> duration)
    {
        // only record tracks that have been played for at least of few seconds...
        if (duration && *duration < std::chrono::seconds{ 5 })
            return;

        addTimedListen({ listen, Wt::WDateTime::currentDateTime() });
    }

    void InternalBackend::addTimedListen(const TimedListen& listen)
    {
        db::Session& session{ _db.getTLSSession() };
        auto transaction{ session.createWriteTransaction() };

        if (db::Listen::find(session, listen.userId, listen.trackId, db::ScrobblingBackend::Internal, listen.listenedAt))
            return;

        const db::User::pointer user{ db::User::find(session, listen.userId) };
        if (!user)
            return;

        const db::Track::pointer track{ db::Track::find(session, listen.trackId) };
        if (!track)
            return;

        auto dbListen{ session.create<db::Listen>(user, track, db::ScrobblingBackend::Internal, listen.listenedAt) };
        dbListen.modify()->setSyncState(db::SyncState::Synchronized);
    }
} // namespace lms::scrobbling
