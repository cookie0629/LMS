#include "services/auth/IPasswordService.hpp"

#include "internal/InternalPasswordService.hpp"
#include "database/IDb.hpp"

namespace lms::auth
{
    std::unique_ptr<IPasswordService> createPasswordService(
        std::string_view backend,
        db::IDb& db,
        std::size_t maxThrottlerEntryCount)
    {
        if (backend == "internal")
        {
            return std::make_unique<InternalPasswordService>(db, maxThrottlerEntryCount);
        }

        throw Exception{ "Unknown password service backend: " + std::string{ backend } };
    }
} // namespace lms::auth

