// 筛选条件/查询过滤器定义

#pragma once

#include <span>
#include <vector>

#include "database/objects/ClusterId.hpp"
#include "database/objects/LabelId.hpp"
#include "database/objects/MediaLibraryId.hpp"
#include "database/objects/ReleaseTypeId.hpp"

namespace lms::db
{
    struct Filters
    {
        MediaLibraryId mediaLibrary;     // tracks that belongs to this library
        std::vector<ClusterId> clusters; // tracks that belong to all these clusters
        LabelId label;                   // tracks which release has this label
        ReleaseTypeId releaseType;       // tracks which release has this type

        Filters& setClusters(std::span<const ClusterId> _clusters)
        {
            clusters.assign(std::cbegin(_clusters), std::cend(_clusters));
            return *this;
        }
        Filters& setMediaLibrary(MediaLibraryId _mediaLibrary)
        {
            mediaLibrary = _mediaLibrary;
            return *this;
        }
        Filters& setLabel(LabelId _label)
        {
            label = _label;
            return *this;
        }
        Filters& setReleaseType(ReleaseTypeId _releaseType)
        {
            releaseType = _releaseType;
            return *this;
        }
    };
} // namespace lms::db