/*
 * Copyright (C) 2018 Emeric Poupon
 *
 * This file is part of LMS.
 *
 * LMS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LMS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Auth.hpp"

#include <Wt/Auth/HashFunction.h>

#include <Wt/WCheckBox.h>
#include <Wt/WEnvironment.h>
#include <Wt/WFormModel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WRandom.h>

#include "core/Service.hpp"
#include "database/Session.hpp"
#include "database/objects/User.hpp"
#include "services/auth/IAuthTokenService.hpp"
#include "services/auth/IPasswordService.hpp"

#include "LmsApplication.hpp"
#include "common/LoginNameValidator.hpp"
#include "common/MandatoryValidator.hpp"
#include "common/PasswordValidator.hpp"

namespace lms::ui
{
    namespace
    {
        static constexpr core::LiteralString authTokenDomain{ "ui" };
        static const std::string authCookieName{ "LmsAuth" };
        static const std::string authCookieSalt{ Wt::Auth::SHA1HashFunction{}.compute(authCookieName, authTokenDomain.c_str()) };

        // createAuthToken: 创建"记住我"认证令牌并设置 HTTP cookie。
        // createAuthToken: создаёт токен аутентификации "запомнить меня" и устанавливает HTTP cookie.
        // 流程：
        // 1. 生成随机 64 字符的令牌
        // 2. 使用 SHA1 哈希函数对令牌进行哈希（使用盐值）
        // 3. 将哈希值存储到数据库
        // 4. 将原始令牌设置为 HTTP cookie（仅在 HTTPS 时设置安全标志）
        // Процесс:
        // 1. Генерирует случайный токен из 64 символов
        // 2. Хеширует токен с помощью SHA1 (с использованием соли)
        // 3. Сохраняет хеш в БД
        // 4. Устанавливает исходный токен как HTTP cookie (флаг безопасности только при HTTPS)
        void createAuthToken(db::UserId userId, const Wt::WDateTime& expiry)
        {
            const std::string authCookie{ Wt::WRandom::generateId(64) };
            const std::string hashedAuthCookie{ Wt::Auth::SHA1HashFunction{}.compute(authCookie, authCookieSalt) };
            core::Service<auth::IAuthTokenService>::get()->createAuthToken(authTokenDomain, userId, hashedAuthCookie);

            LmsApp->setCookie(authCookieName,
                              authCookie,
                              expiry.toTime_t() - Wt::WDateTime::currentDateTime().toTime_t(),
                              "",
                              "",
                              LmsApp->environment().urlScheme() == "https");
        }

        // AuthModel: 登录表单的数据模型，处理字段验证和用户认证。
        // AuthModel: модель данных формы входа, обрабатывает валидацию полей и аутентификацию пользователя.
        class AuthModel : public Wt::WFormModel
        {
        public:
            // Associate each field with a unique string literal.
            // 将每个字段与唯一的字符串字面量关联
            // Связывает каждое поле с уникальным строковым литералом
            static const Field LoginNameField;
            static const Field PasswordField;
            static const Field RememberMeField;

            AuthModel(auth::IPasswordService& passwordService)
                : _passwordService{ passwordService }
            {
                // 添加表单字段并设置验证器
                // Добавляем поля формы и устанавливаем валидаторы
                addField(LoginNameField);
                addField(PasswordField);
                addField(RememberMeField);

                setValidator(LoginNameField, createLoginNameValidator());
                setValidator(PasswordField, createMandatoryValidator());
            }

            // saveData: 保存登录数据：更新用户最后登录时间，如果勾选了"记住我"则创建认证令牌。
            // saveData: сохраняет данные входа: обновляет время последнего входа пользователя, создаёт токен аутентификации, если отмечено "запомнить меня".
            void saveData()
            {
                bool isDemo;
                {
                    auto transaction{ LmsApp->getDbSession().createWriteTransaction() };

                    // 查找用户并更新最后登录时间
                    // Находим пользователя и обновляем время последнего входа
                    db::User::pointer user{ db::User::find(LmsApp->getDbSession(), valueText(LoginNameField).toUTF8()) };
                    user.modify()->setLastLogin(Wt::WDateTime::currentDateTime());
                    _userId = user->getId();

                    isDemo = user->isDemo();
                }

                // 如果勾选了"记住我"，创建认证令牌（演示用户 3 天，普通用户 1 年）
                // Если отмечено "запомнить меня", создаём токен аутентификации (для демо-пользователя 3 дня, для обычного — 1 год)
                if (Wt::asNumber(value(RememberMeField)))
                {
                    const Wt::WDateTime now{ Wt::WDateTime::currentDateTime() };
                    createAuthToken(_userId, isDemo ? now.addDays(3) : now.addYears(1));
                }
            }

            // validateField: 验证表单字段。对密码字段进行实际认证检查。
            // validateField: валидирует поле формы. Для поля пароля выполняет фактическую проверку аутентификации.
            bool validateField(Field field)
            {
                Wt::WString error;

                if (field == PasswordField)
                {
                    // 调用密码服务验证用户名和密码
                    // Вызываем сервис паролей для проверки имени пользователя и пароля
                    const auto checkResult{ _passwordService.checkUserPassword(
                        boost::asio::ip::make_address(LmsApp->environment().clientAddress()),
                        valueText(LoginNameField).toUTF8(),
                        valueText(PasswordField).toUTF8()) };
                    switch (checkResult.state)
                    {
                    case auth::IPasswordService::CheckResult::State::Granted:
                        // 认证成功：保存用户 ID
                        // Аутентификация успешна: сохраняем ID пользователя
                        _userId = checkResult.userId;
                        break;
                    case auth::IPasswordService::CheckResult::State::Denied:
                        // 认证失败：用户名或密码错误
                        // Аутентификация отклонена: неверное имя пользователя или пароль
                        error = Wt::WString::tr("Lms.password-bad-login-combination");
                        break;
                    case auth::IPasswordService::CheckResult::State::Throttled:
                        // 请求被限流：登录尝试过于频繁
                        // Запрос ограничен: слишком частые попытки входа
                        error = Wt::WString::tr("Lms.password-client-throttled");
                        break;
                    }
                }
                else
                {
                    return Wt::WFormModel::validateField(field);
                }

                setValidation(field, Wt::WValidator::Result(error.empty() ? Wt::ValidationState::Valid : Wt::ValidationState::Invalid, error));

                return (validation(field).state() == Wt::ValidationState::Valid);
            }

            db::UserId getUserId() const { return _userId; }

        private:
            db::UserId _userId;
            auth::IPasswordService& _passwordService;
        };

        const AuthModel::Field AuthModel::LoginNameField{ "login-name" };
        const AuthModel::Field AuthModel::PasswordField{ "password" };
        const AuthModel::Field AuthModel::RememberMeField{ "remember-me" };
    } // namespace

    // processAuthToken: 处理"记住我"cookie，验证认证令牌并返回用户 ID。
    // processAuthToken: обрабатывает cookie "запомнить меня", проверяет токен аутентификации и возвращает ID пользователя.
    // 流程：
    // 1. 从 cookie 中读取认证令牌
    // 2. 对令牌进行哈希并与数据库中的哈希值比较
    // 3. 如果验证成功，创建新令牌（单次使用令牌需要刷新）
    // 4. 如果验证失败或被限流，清除 cookie
    // Процесс:
    // 1. Читает токен аутентификации из cookie
    // 2. Хеширует токен и сравнивает с хешем в БД
    // 3. При успешной проверке создаёт новый токен (одноразовые токены требуют обновления)
    // 4. При неудаче или ограничении очищает cookie
    db::UserId processAuthToken(const Wt::WEnvironment& env)
    {
        const std::string* authCookie{ env.getCookie(authCookieName) };
        if (!authCookie)
            return db::UserId{};

        const std::string hashedCookie{ Wt::Auth::SHA1HashFunction{}.compute(*authCookie, authCookieSalt) };

        const auto res{ core::Service<auth::IAuthTokenService>::get()->processAuthToken(authTokenDomain, boost::asio::ip::make_address(env.clientAddress()), hashedCookie) };
        switch (res.state)
        {
        case auth::IAuthTokenService::AuthTokenProcessResult::State::Denied:
        case auth::IAuthTokenService::AuthTokenProcessResult::State::Throttled:
            // 验证失败或被限流：清除无效的 cookie
            // Проверка не пройдена или ограничена: очищаем недействительный cookie
            LmsApp->setCookie(authCookieName, std::string{}, 0, "", "", env.urlScheme() == "https");
            return db::UserId{};

        case auth::IAuthTokenService::AuthTokenProcessResult::State::Granted:
            // 验证成功：单次使用令牌需要创建新令牌以继续使用
            // Проверка успешна: одноразовые токены требуют создания нового токена для продолжения использования
            assert(res.authTokenInfo->maxUseCount && res.authTokenInfo->maxUseCount.value() == 1); // single-use token
            createAuthToken(res.authTokenInfo->userId, res.authTokenInfo->expiry);
            break;
        }

        return res.authTokenInfo->userId;
    }

    void clearAuthTokens(db::UserId userId)
    {
        core::Service<auth::IAuthTokenService>::get()->clearAuthTokens(authTokenDomain, userId);
    }

    // PasswordAuth 构造函数：创建登录表单视图，设置字段、验证器和事件处理。
    // Конструктор PasswordAuth: создаёт представление формы входа, настраивает поля, валидаторы и обработку событий.
    PasswordAuth::PasswordAuth(auth::IPasswordService& passwordService)
        : Wt::WTemplateFormView{ Wt::WString::tr("Lms.Auth.template") }
    {
        auto model{ std::make_shared<AuthModel>(passwordService) };

        // processAuth: 处理登录提交：验证表单，保存数据，发出登录成功信号。
        // processAuth: обрабатывает отправку формы входа: валидирует форму, сохраняет данные, испускает сигнал успешного входа.
        auto processAuth = [this, model] {
            updateModel(model.get());

            if (model->validate())
            {
                model->saveData();
                assert(model->getUserId().isValid());
                userLoggedIn.emit(model->getUserId());
            }
            else
                updateView(model.get());
        };

        // LoginName
        auto loginName{ std::make_unique<Wt::WLineEdit>() };
        loginName->setAttributeValue("autocomplete", "username");
        setFormWidget(AuthModel::LoginNameField, std::move(loginName));

        // Password
        auto password = std::make_unique<Wt::WLineEdit>();
        password->setEchoMode(Wt::EchoMode::Password);
        password->enterPressed().connect(this, processAuth);
        password->setAttributeValue("autocomplete", "current-password");
        setFormWidget(AuthModel::PasswordField, std::move(password));

        // Remember me
        setFormWidget(AuthModel::RememberMeField, std::make_unique<Wt::WCheckBox>());

        {
            auto transaction{ LmsApp->getDbSession().createReadTransaction() };

            db::User::pointer demoUser{ db::User::findDemoUser(LmsApp->getDbSession()) };
            if (demoUser)
            {
                model->setValue(AuthModel::LoginNameField, demoUser->getLoginName());
                model->setValue(AuthModel::PasswordField, demoUser->getLoginName());
            }
        }

        Wt::WPushButton* loginBtn{ bindNew<Wt::WPushButton>("login-btn", Wt::WString::tr("Lms.Auth.login")) };
        loginBtn->clicked().connect(this, processAuth);

        updateView(model.get());
    }
} // namespace lms::ui
