
#pragma once

#include <string>

#include "core/Exception.hpp"
#include "database/Types.hpp"

namespace lms::auth
{
    class Exception : public core::LmsException
    {
        using core::LmsException::LmsException;
    };

    class NotImplementedException : public Exception
    {
    public:
        NotImplementedException()
            : Exception{ "Not implemented" } {}
    };

    class UserNotFoundException : public Exception
    {
    public:
        UserNotFoundException()
            : Exception{ "User not found" } {}
    };

    struct PasswordValidationContext
    {
        std::string loginName;
        db::UserType userType;
    };

    class PasswordException : public Exception
    {
    public:
        using Exception::Exception;
    };

    class PasswordTooWeakException : public PasswordException
    {
    public:
        PasswordTooWeakException()
            : PasswordException{ "Password too weak" } {}
    };

    class PasswordMustMatchLoginNameException : public PasswordException
    {
    public:
        PasswordMustMatchLoginNameException()
            : PasswordException{ "Password must match login name" } {}
    };
} // namespace lms::auth
