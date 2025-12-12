
#pragma once

#include <chrono>
#include <optional>

#include "services/scrobbling/Listen.hpp"

namespace lms::db
{
    class Session;
    class TrackList;
    class User;
} // namespace lms::db

namespace lms::scrobbling
{
    class IScrobblingBackend
    {
    public:
        virtual ~IScrobblingBackend() = default;

        // Listens
        virtual void listenStarted(const Listen& listen) = 0;
        virtual void listenFinished(const Listen& listen, std::optional<std::chrono::seconds> duration) = 0;
        virtual void addTimedListen(const TimedListen& listen) = 0;
    };
} // namespace lms::scrobbling
