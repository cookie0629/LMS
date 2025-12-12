// 随机数与随机字符串工具实现

#include "core/Random.hpp"

namespace lms::core::random
{

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