
#pragma once

#include <atomic>
#include <filesystem>
#include <functional>

#include "core/LiteralString.hpp"

#include "RefreshContext.hpp"

namespace lms::podcast
{
    class RefreshStep
    {
    public:
        using OnDoneCallback = std::function<void(bool success)>;

        RefreshStep(RefreshContext& context, OnDoneCallback callback)
            : _context{ context }
            , _onDoneCallback{ std::move(callback) } {}
        virtual ~RefreshStep() = default;

        virtual core::LiteralString getName() const = 0;
        virtual void run() = 0;

        void requestAbort(bool value)
        {
            _abortRequested = value;
        }

    protected:
        bool abortRequested() const
        {
            return _abortRequested;
        }

        // Called by the step implementation when done
        void onDone()
        {
            _onDoneCallback(true);
        }

        // Called by the step implementation when it wants to abort the whole refresh process
        void onAbort()
        {
            _onDoneCallback(false);
        }

        Executor& getExecutor()
        {
            return _context.executor;
        }

        db::IDb& getDb()
        {
            return _context.db;
        }

        const std::filesystem::path& getCachePath() const
        {
            return _context.cachePath;
        }

        const std::filesystem::path& getTmpCachePath() const
        {
            return _context.tmpCachePath;
        }

        core::http::IClient& getClient()
        {
            return _context.client;
        }

    private:
        RefreshContext& _context;
        OnDoneCallback _onDoneCallback;
        std::atomic<bool> _abortRequested;
    };

} // namespace lms::podcast