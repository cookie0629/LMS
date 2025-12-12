
#pragma once

#include "RefreshStep.hpp"

namespace lms::podcast
{
    class CheckForMissingFilesStep : public RefreshStep
    {
        using RefreshStep::RefreshStep;

    private:
        core::LiteralString getName() const override;
        void run() override;

        void checkMissingImages();
        void checkMissingEpisodes();
    };
} // namespace lms::podcast