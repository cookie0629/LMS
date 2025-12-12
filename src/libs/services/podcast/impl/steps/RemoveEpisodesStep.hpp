
#pragma once

#include <chrono>

#include "RefreshStep.hpp"

namespace lms::podcast
{
    class RemoveEpisodesStep : public RefreshStep
    {
    public:
        RemoveEpisodesStep(RefreshContext& context, OnDoneCallback callback);

    private:
        core::LiteralString getName() const override;
        void run() override;

        const std::chrono::days _autoDownloadEpisodesMaxAge;
    };
} // namespace lms::podcast