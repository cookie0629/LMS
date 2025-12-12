
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
    //
    // 功能：
    // - 管理用户会话（认证、授权、用户信息）
    // - 管理主界面组件（MediaPlayer、PlayQueue、ModalManager、NotificationContainer）
    // - 处理路由（根据 URL 路径切换视图）
    // - 处理扫描器事件（转发到前端）
    // - 提供数据库访问接口（线程安全的会话）
    // - 管理资源（封面图片资源）
    //
    // Функции:
    // - Управляет пользовательской сессией (аутентификация, авторизация, информация о пользователе)
    // - Управляет компонентами главного интерфейса (MediaPlayer, PlayQueue, ModalManager, NotificationContainer)
    // - Обрабатывает роутинг (переключение представлений по URL-пути)
    // - Обрабатывает события сканера (перенаправление на фронтенд)
    // - Предоставляет интерфейс доступа к БД (потокобезопасная сессия)
    // - Управляет ресурсами (ресурсы изображений обложек)
    class LmsApplication : public Wt::WApplication
    {
    public:
        LmsApplication(const Wt::WEnvironment& env, db::IDb& db, LmsApplicationManager& appManager, AuthenticationBackend authBackend);
        ~LmsApplication();

        // create: 工厂方法，创建新的 LmsApplication 实例。
        // create: фабричный метод для создания нового экземпляра LmsApplication.
        static std::unique_ptr<Wt::WApplication> create(const Wt::WEnvironment& env, db::IDb& db, LmsApplicationManager& appManager, AuthenticationBackend authBackend);
        
        // instance: 获取当前会话的 LmsApplication 实例（单例模式）。
        // instance: получает экземпляр LmsApplication текущей сессии (паттерн Singleton).
        static LmsApplication* instance();

        std::shared_ptr<ArtworkResource> getArtworkResource() { return _artworkResource; }
        db::IDb& getDb();
        // 永远线程安全的会话获取接口 / всегда потокобезопасный доступ к сессии
        // 使用线程本地存储（TLS）确保每个线程有独立的数据库会话
        // Использует локальное хранилище потока (TLS) для обеспечения независимой сессии БД для каждого потока
        db::Session& getDbSession();

        db::ObjectPtr<db::User> getUser();
        db::UserId getUserId() const;
        // 调用前用户必须已登录 / пользователь должен быть залогинен
        bool isUserAuthStrong() const;
        // 调用前用户必须已登录 / пользователь должен быть залогинен
        db::UserType getUserType() const;
        // 调用前用户必须已登录 / пользователь должен быть залогинен
        std::string_view getUserLoginName() const;

        scanner::Events& getScannerEvents() { return _scannerEvents; }

        AuthenticationBackend getAuthBackend() const { return _authBackend; }

        // post: 将函数投递到当前会话的事件循环中异步执行。
        // post: отправляет функцию в цикл событий текущей сессии для асинхронного выполнения.
        void post(std::function<void()> func);
        
        // setTitle: 设置浏览器标签页标题。
        // setTitle: устанавливает заголовок вкладки браузера.
        void setTitle(const Wt::WString& title = "");

        // notifyMsg: 显示通知消息（成功、错误、警告、信息）。
        // notifyMsg: показывает уведомление (успех, ошибка, предупреждение, информация).
        void notifyMsg(Notification::Type type, const Wt::WString& message, std::chrono::milliseconds duration = std::chrono::milliseconds{ 5000 });

        MediaPlayer& getMediaPlayer() const { return *_mediaPlayer; }
        PlayQueue& getPlayQueue() const { return *_playQueue; }
        ModalManager& getModalManager() const { return *_modalManager; }

        // 会话结束前发出的信号（用户可能已经退出登录）/ сигнал, испускаемый перед завершением сессии (пользователь может быть уже разлогинен)
        // 用于清理资源、保存状态等操作
        // Используется для очистки ресурсов, сохранения состояния и т.д.
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
        // UserAuthInfo: 用户认证信息结构，存储当前登录用户的基本信息。
        // UserAuthInfo: структура информации об аутентификации пользователя, хранит основную информацию о текущем залогиненном пользователе.
        struct UserAuthInfo
        {
            db::UserId userId;                      // 用户 ID / ID пользователя
            db::UserType userType{ db::UserType::REGULAR };  // 用户类型（普通/管理员/演示）/ тип пользователя (обычный/админ/демо)
            std::string userLoginName;              // 登录名 / имя для входа
            bool strongAuth{};                      // 是否为强认证（密码登录为 true，token 登录为 false）/ является ли аутентификация сильной (true для входа по паролю, false для токена)
        };
        std::optional<UserAuthInfo> _user;  // 当前用户信息（如果已登录）/ информация о текущем пользователе (если залогинен)
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
