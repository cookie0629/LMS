
#include "DuplicateTracks.hpp"

#include <algorithm>

namespace lms::recommendation::PlaylistGeneratorConstraint
{
    float DuplicateTracks::computeScore(const std::vector<db::TrackId>& trackIds, std::size_t trackIndex)
    {
        const auto count{ std::count(std::cbegin(trackIds), std::cend(trackIds), trackIds[trackIndex]) };
        return count == 1 ? 0 : 1'000;
    }
} // namespace lms::recommendation::PlaylistGeneratorConstraint
