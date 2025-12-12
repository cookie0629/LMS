
#include "ListenBrainzBackend.hpp"

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "core/http/IClient.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/Track.hpp"

#include "Utils.hpp"

namespace lms::scrobbling::listenBrainz
{
    using namespace db;

    namespace
    {
        bool canBeScrobbled(Session& session, TrackId trackId, std::chrono::seconds duration)
        {
            auto transaction{ session.createReadTransaction() };

            const Track::pointer track{ Track::find(session, trackId) };
            if (!track)
                return false;

            const bool res{ duration >= std::chrono::minutes(4) || (duration >= track->getDuration() / 2) };
            if (!res)
                LOG(DEBUG, "Track cannot be scrobbled since played duration is too short: " << duration.count() << "s, total duration = " << std::chrono::duration_cast<std::chrono::seconds>(track->getDuration()).count() << "s");

            return res;
        }
    } // namespace

    ListenBrainzBackend::ListenBrainzBackend(boost::asio::io_context& ioContext, db::IDb& db)
        : _ioContext{ ioContext }
        , _db{ db }
        , _baseAPIUrl{ core::Service<core::IConfig>::get()->getString("listenbrainz-api-base-url", "https://api.listenbrainz.org") }
        , _client{ core::http::createClient(_ioContext, _baseAPIUrl) }
        , _listensSynchronizer{ _ioContext, db, *_client }
    {
        LOG(INFO, "Starting ListenBrainz backend... API endpoint = '" << _baseAPIUrl << "'");
    }

    ListenBrainzBackend::~ListenBrainzBackend()
    {
        LOG(INFO, "Stopped ListenBrainz backend!");
    }

    void ListenBrainzBackend::listenStarted(const Listen& listen)
    {
        _listensSynchronizer.enqueListenNow(listen);
    }

    void ListenBrainzBackend::listenFinished(const Listen& listen, std::optional<std::chrono::seconds> duration)
    {
        if (duration && !canBeScrobbled(_db.getTLSSession(), listen.trackId, *duration))
            return;

        const TimedListen timedListen{ listen, Wt::WDateTime::currentDateTime() };
        _listensSynchronizer.enqueListen(timedListen);
    }

    void ListenBrainzBackend::addTimedListen(const TimedListen& timedListen)
    {
        _listensSynchronizer.enqueListen(timedListen);
    }
} // namespace lms::scrobbling::listenBrainz
