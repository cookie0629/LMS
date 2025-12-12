
#pragma once

#include <algorithm>
#include <functional>

namespace lms::core::utils
{
    template<typename Container, typename T>
    void push_back_if_not_present(Container& container, const T& val)
    {
        if (std::find(std::cbegin(container), std::cend(container), val) == std::cend(container))
            container.push_back(val);
    }

    template<class... Ts>
    struct overloads : Ts...
    {
        using Ts::operator()...;
    };
} // namespace lms::core::utils