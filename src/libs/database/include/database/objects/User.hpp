#pragma once

#include <string>
#include <string_view>
#include <optional>

#include <Wt/Dbo/Field.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WDateTime.h>

#include <Wt/Dbo/collection.h>

#include "core/UUID.hpp"
#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/UserId.hpp"

namespace lms::db
{
    class Session;
    class AuthToken;

    /**
     * @brief 用户数据对象
     */
    class User final : public Object<User, UserId>
    {
    public:
        /**
         * @brief 密码哈希结构
         */
        struct PasswordHash
        {
            std::size_t bcryptRoundCount;
            std::string salt;
            std::string hash;
        };

        User() = default;

        /**
         * @brief 获取用户数量
         */
        static std::size_t getCount(Session& session);

        /**
         * @brief 根据ID查找用户
         */
        static pointer find(Session& session, UserId id);

        /**
         * @brief 根据登录名查找用户
         */
        static pointer find(Session& session, std::string_view loginName);

        /**
         * @brief 查找演示用户
         */
        static pointer findDemoUser(Session& session);

        // 访问器
        const std::string& getLoginName() const { return _loginName; }
        PasswordHash getPasswordHash() const 
        { 
            return PasswordHash{ 
                .bcryptRoundCount = static_cast<std::size_t>(_bcryptRoundCount), 
                .salt = _passwordSalt, 
                .hash = _passwordHash 
            }; 
        }
        const Wt::WDateTime& getLastLogin() const { return _lastLogin; }

        // 写入方法
        void setLastLogin(const Wt::WDateTime& dateTime) { _lastLogin = dateTime; }
        void setPasswordHash(const PasswordHash& passwordHash)
        {
            _bcryptRoundCount = static_cast<int>(passwordHash.bcryptRoundCount);
            _passwordSalt = passwordHash.salt;
            _passwordHash = passwordHash.hash;
        }
        void setType(UserType type) { _type = type; }

        // 读取方法
        bool isAdmin() const { return _type == UserType::ADMIN; }
        bool isDemo() const { return _type == UserType::DEMO; }
        UserType getType() const { return _type; }

        /**
         * @brief 获取认证令牌数量
         */
        std::size_t getAuthTokensCount() const { return _authTokens.size(); }

        /**
         * @brief 持久化方法（Wt::Dbo 要求）
         */
        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _type, "type");
            Wt::Dbo::field(a, _loginName, "login_name");
            Wt::Dbo::field(a, _bcryptRoundCount, "bcrypt_round_count");
            Wt::Dbo::field(a, _passwordSalt, "password_salt");
            Wt::Dbo::field(a, _passwordHash, "password_hash");
            Wt::Dbo::field(a, _lastLogin, "last_login");
            Wt::Dbo::hasMany(a, _authTokens, Wt::Dbo::ManyToOne, "user");
        }

    private:
        friend class Session;
        
        /**
         * @brief 私有构造函数
         */
        User(std::string_view loginName);

        /**
         * @brief 创建用户（由Session调用）
         */
        static pointer create(Session& session, std::string_view loginName);

        std::string _loginName;
        int _bcryptRoundCount{};
        std::string _passwordSalt;
        std::string _passwordHash;
        Wt::WDateTime _lastLogin;
        UserType _type{ UserType::REGULAR };
        Wt::Dbo::collection<Wt::Dbo::ptr<AuthToken>> _authTokens;
    };
} // namespace lms::db

