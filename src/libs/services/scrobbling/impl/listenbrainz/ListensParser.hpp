
#pragma once

#include <string_view>

#include "ListenTypes.hpp"

namespace lms::scrobbling::listenBrainz
{
    class ListensParser
    {
    public:
        struct Result
        {
            std::size_t listenCount{};   // may be > than listens.size()
            std::vector<Listen> listens; // successfully parsed listens
        };

        static Result parse(std::string_view msgBody);
    };
} // namespace lms::scrobbling::listenBrainz
