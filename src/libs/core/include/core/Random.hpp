#pragma once

#include <algorithm>
#include <cstdint>
#include <random>

namespace lms::core::random
{
    using RandGenerator = std::mt19937;
    
    /**
     * @brief 获取随机数生成器
     * @return 线程局部的随机数生成器引用
     */
    RandGenerator& getRandGenerator();

    /**
     * @brief 创建指定种子的随机数生成器
     * @param seed 种子值
     * @return 随机数生成器
     */
    RandGenerator createSeededGenerator(uint_fast32_t seed);

    /**
     * @brief 获取指定范围内的随机整数
     * @tparam T 整数类型
     * @param min 最小值（包含）
     * @param max 最大值（包含）
     * @return 随机数
     */
    template<typename T>
    T getRandom(T min, T max)
    {
        std::uniform_int_distribution<> dist{ min, max };
        return dist(getRandGenerator());
    }

    /**
     * @brief 获取指定范围内的随机浮点数
     * @tparam T 浮点数类型
     * @param min 最小值（包含）
     * @param max 最大值（包含）
     * @return 随机数
     */
    template<typename T>
    T getRealRandom(T min, T max)
    {
        std::uniform_real_distribution<> dist{ min, max };
        return dist(getRandGenerator());
    }

    /**
     * @brief 随机打乱容器
     * @tparam Container 容器类型
     * @param container 要打乱的容器
     */
    template<typename Container>
    void shuffleContainer(Container& container)
    {
        std::shuffle(std::begin(container), std::end(container), getRandGenerator());
    }

    /**
     * @brief 从容器中随机选择一个元素
     * @tparam Container 容器类型
     * @param container 容器
     * @return 随机元素的迭代器，如果容器为空则返回 end()
     */
    template<typename Container>
    typename Container::const_iterator pickRandom(const Container& container)
    {
        if (container.empty())
            return std::end(container);

        return std::next(std::begin(container), getRandom(0, static_cast<int>(container.size() - 1)));
    }
} // namespace lms::core::random

