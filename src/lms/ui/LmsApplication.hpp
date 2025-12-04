/*
 * Copyright (C) 2013 Emeric Poupon
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

#include <optional>
#include <string>
#include <string_view>

#include <Wt/WApplication.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/UserId.hpp"
#include "services/scanner/ScannerEvents.hpp"

#include "Auth.hpp"
#include "Notification.hpp"

namespace lms::db
{
    class IDb;
    class Session;
    class User;
} // namespace lms::db

namespace lms::ui
{
    class ArtworkResource;
    class LmsApplicationException;
    class MediaPlayer;
    class PlayQueue;
    class LmsApplicationManager;
    class NotificationContainer;
    class ModalManager;

    // LmsApplication: 单个浏览器会话对应的 LMS Web 应用实例，负责会话状态、路由和主界面。
    // LmsApplication: экземпляр веб‑приложения LMS на одну сессию браузера, управляет состоянием, роутингом и главным UI.
    class LmsApplication : public Wt::WApplication
    {
    public:
        LmsApplication(const Wt::WEnvironment& env, db::IDb& db, LmsApplicationManager& appManager, AuthenticationBackend authBackend);
        ~LmsApplication();

        static std::unique_ptr<Wt::WApplication> create(const Wt::WEnvironment& env, db::IDb& db, LmsApplicationManager& appManager, AuthenticationBackend authBackend);
        static LmsApplication* instance();

        std::shared_ptr<ArtworkResource> getArtworkResource() { return _artworkResource; }
        db::IDb& getDb();
        db::Session& getDbSession(); // 永远线程安全的会话获取接口 / всегда потокобезопасный доступ к сессии

        db::ObjectPtr<db::User> getUser();
        db::UserId getUserId() const;
        bool isUserAuthStrong() const;             // 调用前用户必须已登录 / пользователь должен быть залогинен
        db::UserType getUserType() const;          // 调用前用户必须已登录 / пользователь должен быть залогинен
        std::string_view getUserLoginName() const; // 调用前用户必须已登录 / пользователь должен быть залогинен

        scanner::Events& getScannerEvents() { return _scannerEvents; }

        AuthenticationBackend getAuthBackend() const { return _authBackend; }

        void post(std::function<void()> func);
        void setTitle(const Wt::WString& title = "");

        void notifyMsg(Notification::Type type, const Wt::WString& message, std::chrono::milliseconds duration = std::chrono::milliseconds{ 5000 });

        MediaPlayer& getMediaPlayer() const { return *_mediaPlayer; }
        PlayQueue& getPlayQueue() const { return *_playQueue; }
        ModalManager& getModalManager() const { return *_modalManager; }

        // 会话结束前发出的信号（用户可能已经退出登录）/ сигнал, испускаемый перед завершением сессии (пользователь может быть уже разлогинен)
        Wt::Signal<>& preQuit() { return _preQuit; }

    private:
        void init();
        void processPasswordAuth();
        void handleException(LmsApplicationException& e);
        void goHomeAndQuit();

        // Signal slots
        void logoutUser();
        void onUserLoggedIn(db::UserId userId, bool strongAuth);

        void notify(const Wt::WEvent& event) override;
        void finalize() override;

        void setUserInfo(db::UserId userId, bool strongAuth);
        void createHome();

        db::IDb& _db;
        Wt::Signal<> _preQuit;
        LmsApplicationManager& _appManager;
        const AuthenticationBackend _authBackend;
        scanner::Events _scannerEvents;
        struct UserAuthInfo
        {
            db::UserId userId;
            db::UserType userType{ db::UserType::REGULAR };
            std::string userLoginName;
            bool strongAuth{};
        };
        std::optional<UserAuthInfo> _user;
        std::shared_ptr<ArtworkResource> _artworkResource;
        MediaPlayer* _mediaPlayer{};
        PlayQueue* _playQueue{};
        NotificationContainer* _notificationContainer{};
        ModalManager* _modalManager{};
    };

    // LmsApp: 获取当前会话 LmsApplication 单例的便捷宏。
    // LmsApp: удобный макрос для получения экземпляра LmsApplication текущей сессии.
#define LmsApp lms::ui::LmsApplication::instance()

} // namespace lms::ui
