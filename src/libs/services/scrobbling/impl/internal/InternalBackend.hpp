
#pragma once

#include "IScrobblingBackend.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::scrobbling
{
    class InternalBackend final : public IScrobblingBackend
    {
    public:
        InternalBackend(db::IDb& db);

    private:
        void listenStarted(const Listen& listen) override;
        void listenFinished(const Listen& listen, std::optional<std::chrono::seconds> duration) override;
        void addTimedListen(const TimedListen& listen) override;

        db::IDb& _db;
    };
} // namespace lms::scrobbling
