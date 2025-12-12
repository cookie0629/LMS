
#pragma once

#include <Wt/Http/Request.h>

#include <optional>
#include <string>
#include <vector>

#include "core/String.hpp"
#include "database/Types.hpp"

#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{

    template<typename T>
    std::vector<T> getMultiParametersAs(const Wt::Http::ParameterMap& parameterMap, const std::string& paramName)
    {
        std::vector<T> res;

        auto it = parameterMap.find(paramName);
        if (it == parameterMap.end())
            return res;

        for (const std::string& param : it->second)
        {
            auto value{ core::stringUtils::readAs<T>(param) };
            if (value)
                res.emplace_back(std::move(*value));
        }

        return res;
    }

    template<typename T>
    std::vector<T> getMandatoryMultiParametersAs(const Wt::Http::ParameterMap& parameterMap, const std::string& param)
    {
        std::vector<T> res{ getMultiParametersAs<T>(parameterMap, param) };
        if (res.empty())
            throw RequiredParameterMissingError{ param };

        return res;
    }

    template<typename T>
    std::optional<T> getParameterAs(const Wt::Http::ParameterMap& parameterMap, const std::string& param)
    {
        std::vector<T> params{ getMultiParametersAs<T>(parameterMap, param) };

        if (params.size() != 1)
            return std::nullopt;

        return T{ std::move(params.front()) };
    }

    template<typename T>
    T getMandatoryParameterAs(const Wt::Http::ParameterMap& parameterMap, const std::string& param)
    {
        auto res{ getParameterAs<T>(parameterMap, param) };
        if (!res)
            throw RequiredParameterMissingError{ param };

        return *res;
    }

    bool hasParameter(const Wt::Http::ParameterMap& parameterMap, const std::string& param);
    std::string decodePasswordIfNeeded(const std::string& password);
} // namespace lms::api::subsonic
