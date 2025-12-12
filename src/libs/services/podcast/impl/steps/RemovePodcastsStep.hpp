
#pragma once

#include "RefreshStep.hpp"

namespace lms::podcast
{
    class RemovePodcastsStep : public RefreshStep
    {
        using RefreshStep::RefreshStep;

    private:
        core::LiteralString getName() const override;
        void run() override;
    };
} // namespace lms::podcast