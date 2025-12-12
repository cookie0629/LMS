
#pragma once

#include <algorithm>
#include <random>

namespace lms::core::random
{
    using RandGenerator = std::mt19937;
    RandGenerator& getRandGenerator();

    RandGenerator createSeededGenerator(uint_fast32_t seed);

    template<typename T>
    T getRandom(T min, T max)
    {
        std::uniform_int_distribution<> dist{ min, max };
        return dist(getRandGenerator());
    }

    template<typename T>
    T getRealRandom(T min, T max)
    {
        std::uniform_real_distribution<> dist{ min, max };
        return dist(getRandGenerator());
    }

    template<typename Container>
    void shuffleContainer(Container& container)
    {
        std::shuffle(std::begin(container), std::end(container), getRandGenerator());
    }

    template<typename Container>
    typename Container::const_iterator pickRandom(const Container& container)
    {
        if (container.empty())
            return std::end(container);

        return std::next(std::begin(container), getRandom(0, static_cast<int>(container.size() - 1)));
    }
} // namespace lms::core::random