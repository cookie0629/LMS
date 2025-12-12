
#pragma once

#include <tuple>

namespace lms::core
{
    namespace details
    {
        template<int... Is>
        struct Seq
        {
        };

        template<int N, int... Is>
        struct GenSeq : GenSeq<N - 1, N - 1, Is...>
        {
        };

        template<int... Is>
        struct GenSeq<0, Is...> : Seq<Is...>
        {
        };

        template<typename T, typename Func, int... Is>
        void forEachTypeInTuple(T&& t, Func f, Seq<Is...>)
        {
            auto l = { (f(std::get<Is>(t)), 0)... };
        }
    } // namespace details

    template<typename... Ts, typename Func>
    void forEachTypeInTuple(std::tuple<Ts...> const& t, Func f)
    {
        details::forEachTypeInTuple(t, f, details::GenSeq<sizeof...(Ts)>());
    }
} // namespace lms::core