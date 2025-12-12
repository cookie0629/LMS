
#pragma once

#include <ostream>
#include <vector>

#include "Network.hpp"

namespace lms::som
{
    class DataNormalizer
    {
    public:
        struct MinMax
        {
            InputVector::value_type min;
            InputVector::value_type max;
        };

        DataNormalizer(std::size_t inputDimCount);

        std::size_t getInputDimCount() const { return _inputDimCount; }
        const MinMax& getValue(std::size_t index) const;

        void setValue(std::size_t index, const MinMax& minMax);

        void computeNormalizationFactors(const std::vector<InputVector>& dataSamples);

        void normalizeData(InputVector& data) const;

        void dump(std::ostream& os) const;

    private:
        InputVector::value_type normalizeValue(InputVector::value_type value, std::size_t dimensionId) const;

        const std::size_t _inputDimCount;

        std::vector<MinMax> _minmax; // Indexed min/max used to normalize data
    };
} // namespace lms::som
