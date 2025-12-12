// 曲目特征/标签实体定义

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <Wt/Dbo/Field.h>

#include "database/IdType.hpp"
#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/TrackId.hpp"

LMS_DECLARE_IDTYPE(TrackFeaturesId)

namespace lms::db
{
    class Session;
    class Track;

    using FeatureName = std::string;
    using FeatureValues = std::vector<double>;
    using FeatureValuesMap = std::unordered_map<FeatureName, FeatureValues>;

    class TrackFeatures final : public Object<TrackFeatures, TrackFeaturesId>
    {
    public:
        TrackFeatures() = default;

        // Find utilities
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, TrackFeaturesId id);
        static pointer find(Session& session, TrackId trackId);
        static RangeResults<TrackFeaturesId> find(Session& session, std::optional<Range> range = std::nullopt);

        FeatureValues getFeatureValues(const FeatureName& feature) const;
        FeatureValuesMap getFeatureValuesMap(const std::unordered_set<FeatureName>& featureNames) const;

        // Accessors
        Wt::Dbo::ptr<Track> getTrack() const { return _track; }

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _data, "data");
            Wt::Dbo::belongsTo(a, _track, "track", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        TrackFeatures(ObjectPtr<Track> track, const std::string& jsonEncodedFeatures);
        static pointer create(Session& session, ObjectPtr<Track> track, const std::string& jsonEncodedFeatures);

        std::string _data;
        Wt::Dbo::ptr<Track> _track;
    };

} // namespace lms::db
