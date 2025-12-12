
#pragma once

#include <optional>
#include <string>

#include <boost/asio/io_context.hpp>

#include "IScrobblingBackend.hpp"
#include "ListensSynchronizer.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::scrobbling::listenBrainz
{
    class ListenBrainzBackend final : public IScrobblingBackend
    {
    public:
        ListenBrainzBackend(boost::asio::io_context& ioContext, db::IDb& db);
        ~ListenBrainzBackend() override;

    private:
        ListenBrainzBackend(const ListenBrainzBackend&) = delete;
        ListenBrainzBackend& operator=(const ListenBrainzBackend&) = delete;

        void listenStarted(const Listen& listen) override;
        void listenFinished(const Listen& listen, std::optional<std::chrono::seconds> duration) override;
        void addTimedListen(const TimedListen& listen) override;

        // Submit listens
        void enqueListen(const Listen& listen, const Wt::WDateTime& timePoint);

        boost::asio::io_context& _ioContext;
        db::IDb& _db;
        std::string _baseAPIUrl;
        std::unique_ptr<core::http::IClient> _client;
        ListensSynchronizer _listensSynchronizer;
    };
} // namespace lms::scrobbling::listenBrainz
