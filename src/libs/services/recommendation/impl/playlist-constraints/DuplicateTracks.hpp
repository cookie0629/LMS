
#pragma once

#include "IConstraint.hpp"

namespace lms::recommendation::PlaylistGeneratorConstraint
{
    class DuplicateTracks : public IConstraint
    {
    private:
        float computeScore(const std::vector<db::TrackId>& trackIds, std::size_t trackIndex) override;
    };
} // namespace lms::recommendation::PlaylistGeneratorConstraint
