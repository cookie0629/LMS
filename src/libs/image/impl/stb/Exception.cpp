
#include "Exception.hpp"

#include "StbImage.hpp"

namespace lms::image
{
    namespace
    {
        std::string getLastStbiFailureReason()
        {
            const char* failureReason{ ::stbi_failure_reason() };
            return failureReason ? failureReason : "unknown reason";
        }
    } // namespace

    StbiException::StbiException(std::string_view desc)
        : Exception{ std::string{ desc } + ": " + getLastStbiFailureReason() }
    {
    }
} // namespace lms::image