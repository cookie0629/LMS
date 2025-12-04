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

#pragma once

#include <Wt/WTemplateFormView.h>

#include "database/objects/UserId.hpp"

namespace lms::auth
{
    class IPasswordService;
}

namespace lms::ui
{
    // AuthenticationBackend: 指定 UI 使用哪种认证后端（内部 / 环境变量 / PAM）。
    // AuthenticationBackend: определяет, какой бэкенд аутентификации использует UI (внутренний, Env, PAM).
    enum class AuthenticationBackend
    {
        Internal,
        Env,
        PAM,
    };

    // processAuthToken: 处理“记住我”cookie，并返回已认证的用户 ID（可能为空）。
    // processAuthToken: обрабатывает cookie "запомнить меня" и возвращает ID пользователя (или пустой).
    db::UserId processAuthToken(const Wt::WEnvironment& env);

    // PasswordAuth: 基于模板的登录表单视图，完成密码校验并发出 userLoggedIn 信号。
    // PasswordAuth: представление формы логина на шаблоне, проверяет пароль и испускает сигнал userLoggedIn.
    class PasswordAuth : public Wt::WTemplateFormView
    {
    public:
        PasswordAuth(auth::IPasswordService& passwordService);

        Wt::Signal<db::UserId> userLoggedIn;
    };
} // namespace lms::ui
