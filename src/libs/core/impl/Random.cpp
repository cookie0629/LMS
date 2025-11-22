#include "core/Random.hpp"

#include <random>

namespace lms::core::random
{
    using RandGenerator = std::mt19937;

    RandGenerator& getRandGenerator()
    {
        static thread_local std::random_device rd;
        static thread_local RandGenerator randGenerator(rd());

        return randGenerator;
    }

    RandGenerator createSeededGenerator(uint_fast32_t seed)
    {
        return RandGenerator{ seed };
    }
} // namespace lms::core::random

