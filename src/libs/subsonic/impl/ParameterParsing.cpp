
#include "ParameterParsing.hpp"

namespace lms::api::subsonic
{
    bool hasParameter(const Wt::Http::ParameterMap& parameterMap, const std::string& param)
    {
        return parameterMap.find(param) != std::cend(parameterMap);
    }

    std::string decodePasswordIfNeeded(const std::string& password)
    {
        if (password.find("enc:") == 0)
        {
            auto decodedPassword{ core::stringUtils::stringFromHex(password.substr(4)) };
            if (!decodedPassword)
                return password; // fallback on plain password

            return *decodedPassword;
        }

        return password;
    }
} // namespace lms::api::subsonic