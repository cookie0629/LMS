
#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lms::recommendation
{

    using FeatureName = std::string;
    using FeatureNames = std::unordered_set<FeatureName>;
    using FeatureValue = double;
    using FeatureValues = std::vector<FeatureValue>;
    using FeatureValuesMap = std::unordered_map<FeatureName, FeatureValues>;

    struct FeatureDef
    {
        std::size_t nbDimensions{};
    };

    FeatureDef getFeatureDef(const FeatureName& featureName);
    FeatureNames getFeatureNames();

    struct FeatureSettings
    {
        double weight{};
    };
    using FeatureSettingsMap = std::unordered_map<FeatureName, FeatureSettings>;

} // namespace lms::recommendation
