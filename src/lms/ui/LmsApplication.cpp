
#include "LmsApplication.hpp"

#include <Wt/WAnchor.h>
#include <Wt/WEnvironment.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WServer.h>
#include <Wt/WStackedWidget.h>

#include "core/ILogger.hpp"
#include "core/ITraceLogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/IQueryPlanRecorder.hpp"
#include "database/Session.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/Cluster.hpp"
#include "database/objects/Release.hpp"
#include "database/objects/TrackList.hpp"
#include "database/objects/User.hpp"
#include "services/artwork/IArtworkService.hpp"
#include "services/auth/IAuthTokenService.hpp"
#include "services/auth/IEnvService.hpp"
#include "services/auth/IPasswordService.hpp"
#include "services/scrobbling/IScrobblingService.hpp"

#include "Auth.hpp"
#include "LmsApplicationException.hpp"
#include "LmsApplicationManager.hpp"
#include "LmsTheme.hpp"
#include "MediaPlayer.hpp"
#include "ModalManager.hpp"
#include "NotificationContainer.hpp"
#include "PlayQueue.hpp"
#include "SettingsView.hpp"
#include "admin/About.hpp"
#include "admin/DebugToolsView.hpp"
#include "admin/InitWizardView.hpp"
#include "admin/MediaLibrariesView.hpp"
#include "admin/ScanSettingsView.hpp"
#include "admin/ScannerController.hpp"
#include "admin/UserView.hpp"
#include "admin/UsersView.hpp"
#include "common/Template.hpp"
#include "explore/Explore.hpp"
#include "explore/Filters.hpp"
#include "resource/ArtworkResource.hpp"
#include "resource/AudioTranscodingResource.hpp"

namespace lms::ui
{
    namespace
    {
        constexpr const char* defaultPath{ "/releases" };

        // createMessageResourceBundle: 加载所有 UI 模板所需的多语言资源。
        // createMessageResourceBundle: загружает все ресурсные файлы сообщений для UI‑шаблонов.
        // 这些资源文件包含所有界面文本的翻译，支持多语言切换。
        // Эти файлы ресурсов содержат переводы всех текстов интерфейса, поддерживают переключение языков.
        std::shared_ptr<Wt::WMessageResourceBundle> createMessageResourceBundle()
        {
            const std::string appRoot{ Wt::WApplication::appRoot() };

            auto res{ std::make_shared<Wt::WMessageResourceBundle>() };
            res->use(appRoot + "admin-about");
            res->use(appRoot + "admin-db");
            res->use(appRoot + "admin-debugtools");
            res->use(appRoot + "admin-initwizard");
            res->use(appRoot + "admin-medialibraries");
            res->use(appRoot + "admin-medialibrary");
            res->use(appRoot + "admin-scannercontroller");
            res->use(appRoot + "admin-scansettings");
            res->use(appRoot + "admin-tracing");
            res->use(appRoot + "admin-user");
            res->use(appRoot + "admin-users");
            res->use(appRoot + "artist");
            res->use(appRoot + "artists");
            res->use(appRoot + "error");
            res->use(appRoot + "explore");
            res->use(appRoot + "login");
            res->use(appRoot + "main");
            res->use(appRoot + "mediaplayer");
            res->use(appRoot + "messages");
            res->use(appRoot + "misc");
            res->use(appRoot + "notifications");
            res->use(appRoot + "playqueue");
            res->use(appRoot + "release");
            res->use(appRoot + "releases");
            res->use(appRoot + "settings");
            res->use(appRoot + "tracklist");
            res->use(appRoot + "tracklists");
            res->use(appRoot + "tracks");

            return res;
        }

        // getOrCreateMessageBundle: 全局缓存 message bundle，避免每个会话重复加载。
        // getOrCreateMessageBundle: кэширует bundle сообщений, чтобы не загружать его для каждой сессии заново.
        // 使用静态变量实现单例模式，所有会话共享同一个资源包。
        // Использует статическую переменную для реализации паттерна Singleton, все сессии используют один и тот же пакет ресурсов.
        std::shared_ptr<Wt::WMessageResourceBundle> getOrCreateMessageBundle()
        {
            static std::shared_ptr<Wt::WMessageResourceBundle> res{ createMessageResourceBundle() };
            return res;
        }

        // createLocale: 从翻译资源中读取本地化格式，创建自定义 WLocale。
        // createLocale: создаёт WLocale, заполняя формат дат/времени из ресурсов перевода.
        // 设置日期、时间、数字格式等本地化信息，确保界面显示符合用户的语言习惯。
        // Настраивает локализацию для дат, времени, чисел и т.д., чтобы интерфейс отображался согласно языковым предпочтениям пользователя.
        Wt::WLocale createLocale(const std::string& name)
        {
            Wt::WLocale locale{ name };
            locale.setDecimalPoint(Wt::WString::tr("Lms.locale.decimal-point").toUTF8());
            locale.setGroupSeparator(Wt::WString::tr("Lms.locale.group-separator").toUTF8());
            locale.setDateFormat(Wt::WString::tr("Lms.locale.date-format").toUTF8());
            locale.setTimeFormat(Wt::WString::tr("Lms.locale.time-format").toUTF8());
            locale.setDateTimeFormat(Wt::WString::tr("Lms.locale.date-time-format").toUTF8());

            return locale;
        }

        enum IdxRoot
        {
            IdxExplore = 0,
            IdxPlayQueue,
            IdxSettings,
            IdxAdminLibraries,
            IdxAdminScanSettings,
            IdxAdminScanner,
            IdxAdminUsers,
            IdxAdminUser,
            IdxAdminDebugTools,
        };

        // handlePathChange: 根据 internalPath 切换主内容区的页，并更新导航高亮。
        // handlePathChange: переключает текущую страницу в mainStack и обновляет активный пункт меню.
        // 功能：
        // 1. 解析当前 URL 路径（如 /artists, /releases 等）
        // 2. 切换到对应的视图页面
        // 3. 更新页面标题
        // 4. 通过 JavaScript 高亮当前导航项
        // Функции:
        // 1. Парсит текущий URL-путь (например, /artists, /releases и т.д.)
        // 2. Переключается на соответствующую страницу представления
        // 3. Обновляет заголовок страницы
        // 4. Выделяет текущий пункт навигации через JavaScript
        void handlePathChange(Wt::WStackedWidget& stack, bool isAdmin)
        {
            static const struct
            {
                std::string path;
                int index;
                bool admin;
                std::optional<Wt::WString> title;
            } views[] = {
                { "/artists", IdxExplore, false, Wt::WString::tr("Lms.Explore.artists") },
                { "/artist", IdxExplore, false, std::nullopt },
                { "/releases", IdxExplore, false, Wt::WString::tr("Lms.Explore.releases") },
                { "/release", IdxExplore, false, std::nullopt },
                { "/tracks", IdxExplore, false, Wt::WString::tr("Lms.Explore.tracks") },
                { "/tracklists", IdxExplore, false, Wt::WString::tr("Lms.Explore.tracklists") },
                { "/tracklist", IdxExplore, false, std::nullopt },
                { "/playqueue", IdxPlayQueue, false, Wt::WString::tr("Lms.PlayQueue.playqueue") },
                { "/settings", IdxSettings, false, Wt::WString::tr("Lms.Settings.settings") },
                { "/admin/libraries", IdxAdminLibraries, true, Wt::WString::tr("Lms.Admin.MediaLibraries.media-libraries") },
                { "/admin/scan-settings", IdxAdminScanSettings, true, Wt::WString::tr("Lms.Admin.Database.scan-settings") },
                { "/admin/scanner", IdxAdminScanner, true, Wt::WString::tr("Lms.Admin.ScannerController.scanner") },
                { "/admin/users", IdxAdminUsers, true, Wt::WString::tr("Lms.Admin.Users.users") },
                { "/admin/user", IdxAdminUser, true, std::nullopt },
                { "/admin/debug-tools", IdxAdminDebugTools, true, Wt::WString::tr("Lms.Admin.DebugTools.debug-tools") },
            };

            LMS_LOG(UI, DEBUG, "Internal path changed to '" << wApp->internalPath() << "'");

            for (const auto& view : views)
            {
                if (wApp->internalPathMatches(view.path))
                {
                    if (view.admin && !isAdmin)
                        break;

                    stack.setCurrentIndex(view.index);
                    if (view.title)
                        LmsApp->setTitle(*view.title);

                    LmsApp->doJavaScript(LmsApp->javaScriptClass() + ".updateActiveNav('" + view.path + "')");
                    return;
                }
            }

            wApp->setInternalPath(defaultPath, true);
        }
    } // namespace

    std::unique_ptr<Wt::WApplication> LmsApplication::create(const Wt::WEnvironment& env, db::IDb& db, LmsApplicationManager& appManager, AuthenticationBackend authBackend)
    {
        return std::make_unique<LmsApplication>(env, db, appManager, authBackend);
    }

    LmsApplication* LmsApplication::instance()
    {
        return static_cast<LmsApplication*>(Wt::WApplication::instance());
    }

    db::IDb& LmsApplication::getDb()
    {
        return _db;
    }

    db::Session& LmsApplication::getDbSession()
    {
        return _db.getTLSSession();
    }

    db::User::pointer LmsApplication::getUser()
    {
        if (!_user)
            return {};

        return db::User::find(getDbSession(), _user->userId);
    }

    db::UserId LmsApplication::getUserId() const
    {
        assert(_user);
        return _user->userId;
    }

    bool LmsApplication::isUserAuthStrong() const
    {
        assert(_user);
        return _user->strongAuth;
    }

    db::UserType LmsApplication::getUserType() const
    {
        assert(_user);
        return _user->userType;
    }

    std::string_view LmsApplication::getUserLoginName() const
    {
        assert(_user);
        return _user->userLoginName;
    }

    LmsApplication::LmsApplication(const Wt::WEnvironment& env, db::IDb& db, LmsApplicationManager& appManager, AuthenticationBackend authBackend)
        : Wt::WApplication{ env }
        , _db{ db }
        , _appManager{ appManager }
        , _authBackend{ authBackend }
    {
        try
        {
            init();
        }
        catch (LmsApplicationException& e)
        {
            LMS_LOG(UI, WARNING, "Caught a LmsApplication exception: " << e.what());
            handleException(e);
        }
        catch (std::exception& e)
        {
            LMS_LOG(UI, ERROR, "Caught exception: " << e.what());
            throw core::LmsException{ "Internal error" }; // Do not put details here at it may appear on the user rendered html
        }
    }

    LmsApplication::~LmsApplication() = default;

    // init: 初始化 Web 应用：设置主题、样式、JavaScript、本地化和事件处理。
    // init: инициализирует веб-приложение: настраивает тему, стили, JavaScript, локализацию и обработку событий.
    void LmsApplication::init()
    {
        LMS_SCOPED_TRACE_OVERVIEW("UI", "ApplicationInit");

        // 设置自定义主题
        // Устанавливаем пользовательскую тему
        setTheme(std::make_shared<LmsTheme>());

        // 加载 Font Awesome 图标库和媒体播放器 JavaScript
        // Загружаем библиотеку иконок Font Awesome и JavaScript медиаплеера
        useStyleSheet("resources/font-awesome/css/font-awesome.min.css");
        require("js/mediaplayer.js");

        // 设置标题、多语言资源和本地化
        // Настраиваем заголовок, ресурсы многоязычности и локализацию
        setTitle();
        setLocalizedStrings(getOrCreateMessageBundle());
        setLocale(createLocale(Wt::WLocale::currentLocale().name()));

        // Handle Media Scanner events and other session events
        // 启用服务器推送更新，用于处理媒体扫描器事件和其他会话事件
        // Включаем серверные push-обновления для обработки событий сканера медиа и других событий сессии
        enableUpdates(true);

        db::UserId userId;
        switch (_authBackend)
        {
        case AuthenticationBackend::Env:
            {
                const auto checkResult{ core::Service<auth::IEnvService>::get()->processEnv(environment()) };
                if (checkResult.state != auth::IEnvService::CheckResult::State::Granted)
                {
                    LMS_LOG(UI, ERROR, "Cannot authenticate user from environment!");
                    throw core::LmsException{ "Cannot authenticate user from environment!" }; // Do not put details here at it may appear on the user rendered html
                }
                assert(checkResult.userId.isValid());
                userId = checkResult.userId;
            }
            break;

        case AuthenticationBackend::Internal:
            [[fallthrough]];
        case AuthenticationBackend::PAM:
            userId = processAuthToken(environment());
            break;
        }

        if (userId.isValid())
            onUserLoggedIn(userId, false /* strongAuth */);
        else
            processPasswordAuth();
    }

    // processPasswordAuth: 处理基于密码的认证流程。
    // processPasswordAuth: обрабатывает процесс аутентификации на основе пароля.
    // 如果是首次连接且使用内部认证，显示初始化向导；否则显示登录表单。
    // Если это первое подключение и используется внутренняя аутентификация, показывает мастер инициализации; иначе показывает форму входа.
    void LmsApplication::processPasswordAuth()
    {
        bool firstConnection{};
        {
            auto transaction{ getDbSession().createReadTransaction() };
            firstConnection = db::User::getCount(getDbSession()) == 0;
        }

        LMS_LOG(UI, DEBUG, "Creating root widget. First connection = " << firstConnection);

        assert(_authBackend == AuthenticationBackend::Internal || _authBackend == AuthenticationBackend::PAM);
        auth::IPasswordService& passwordService{ *core::Service<auth::IPasswordService>::get() };

        // 首次连接：显示初始化向导，让管理员创建第一个用户
        // Первое подключение: показываем мастер инициализации для создания первого пользователя администратором
        if (firstConnection && _authBackend == AuthenticationBackend::Internal)
        {
            root()->addNew<InitWizardView>(passwordService);
            return;
        }

        // 显示登录表单，连接登录成功信号
        // Показываем форму входа, подключаем сигнал успешного входа
        PasswordAuth* auth{ root()->addNew<PasswordAuth>(passwordService) };
        auth->userLoggedIn.connect(this, [this](db::UserId userId) {
            onUserLoggedIn(userId, true /* strongAuth */);
        });
    }

    void LmsApplication::finalize()
    {
        if (_user)
            _appManager.unregisterApplication(*this);

        preQuit().emit();
    }

    void LmsApplication::handleException(LmsApplicationException& e)
    {
        root()->clear();
        Wt::WTemplate* t{ root()->addNew<Wt::WTemplate>(Wt::WString::tr("Lms.Error.template")) };
        t->addFunction("tr", &Wt::WTemplate::Functions::tr);

        t->bindString("error", e.what(), Wt::TextFormat::Plain);
        Wt::WPushButton* btn{ t->bindNew<Wt::WPushButton>("btn-go-home", Wt::WString::tr("Lms.Error.go-home")) };
        btn->clicked().connect([this]() {
            redirect(defaultPath);
        });
    }

    void LmsApplication::goHomeAndQuit()
    {
        WApplication::quit("");
        redirect(".");
    }

    void LmsApplication::logoutUser()
    {
        core::Service<auth::IAuthTokenService>::get()->clearAuthTokens("ui", getUserId());
        LMS_LOG(UI, INFO, "User '" << getUserLoginName() << " 'logged out");
        goHomeAndQuit();
    }

    // onUserLoggedIn: 用户登录成功后的处理：设置用户信息、注册应用、检查重复会话、创建主界面。
    // onUserLoggedIn: обработка после успешного входа пользователя: установка информации о пользователе, регистрация приложения, проверка дублирующих сессий, создание главного интерфейса.
    void LmsApplication::onUserLoggedIn(db::UserId userId, bool strongAuth)
    {
        root()->clear();

        setUserInfo(userId, strongAuth);

        LMS_LOG(UI, INFO, "User '" << getUserLoginName() << "' logged in from '" << environment().clientAddress() << "', user agent = " << environment().userAgent() << ", locale = '" << locale().name() << "'");

        // 注册当前应用实例到应用管理器
        // Регистрируем текущий экземпляр приложения в менеджере приложений
        _appManager.registerApplication(*this);
        
        // 监听新应用注册事件：如果同一用户有多个活动会话，关闭旧会话（演示用户除外）
        // Слушаем событие регистрации нового приложения: если у одного пользователя несколько активных сессий, закрываем старую (кроме демо-пользователя)
        _appManager.applicationRegistered.connect(this, [this](LmsApplication& otherApplication) {
            // Only one active session by user
            if (otherApplication.getUserId() == getUserId())
            {
                if (LmsApp->getUserType() != db::UserType::DEMO)
                {
                    quit(Wt::WString::tr("Lms.quit-other-session"));
                }
            }
        });

        createHome();
    }

    void LmsApplication::setUserInfo(db::UserId userId, bool strongAuth)
    {
        auto transaction{ getDbSession().createReadTransaction() };

        const db::User::pointer user{ db::User::find(getDbSession(), userId) };
        if (!user)
            throw core::LmsException{ "Internal error" }; // Do not put details here at it may appear on the user rendered html

        assert(!_user);
        _user = UserAuthInfo{
            .userId = userId,
            .userType = user->getType(),
            .userLoginName = user->getLoginName(),
            .strongAuth = strongAuth
        };
    }

    // createHome: 创建主界面：初始化资源、JavaScript 函数、导航栏、内容区域和事件连接。
    // createHome: создаёт главный интерфейс: инициализирует ресурсы, JavaScript-функции, панель навигации, область контента и подключения событий.
    void LmsApplication::createHome()
    {
        LMS_SCOPED_TRACE_OVERVIEW("UI", "ApplicationCreateHome");

        // 创建封面资源处理器
        // Создаём обработчик ресурсов обложек
        _artworkResource = std::make_shared<ArtworkResource>();

        // 声明全局 JavaScript 函数：
        // 1. onLoadCover: 当封面图片加载完成时添加 CSS 类
        // 2. updateActiveNav: 更新导航栏中当前活动项的样式
        // Объявляем глобальные JavaScript-функции:
        // 1. onLoadCover: добавляет CSS-класс при завершении загрузки изображения обложки
        // 2. updateActiveNav: обновляет стиль текущего активного пункта в навигации
        declareJavaScriptFunction("onLoadCover", "function(id) { id.className += \" Lms-cover-loaded\"}");
        declareJavaScriptFunction("updateActiveNav",
                                  R"(function(current) {
    const menuItems = document.querySelectorAll('.nav-item a[href]:not([href=""])');
    for (const menuItem of menuItems) {
        if (menuItem.getAttribute("href") === current) {
            menuItem.classList.add('active');
        }
        else {
            menuItem.classList.remove('active');
        }

    }
})");

        Wt::WTemplate* main{ root()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("Lms.main.template"))) };
        main->addFunction("tr", &Wt::WTemplate::Functions::tr);

        Template* navbar{ main->bindNew<Template>("navbar", Wt::WString::tr("Lms.main.template.navbar")) };
        navbar->addFunction("tr", &Wt::WTemplate::Functions::tr);

        _notificationContainer = main->bindNew<NotificationContainer>("notifications");
        _modalManager = main->bindNew<ModalManager>("modal");

        // MediaPlayer
        _mediaPlayer = main->bindNew<MediaPlayer>("player");

        navbar->bindNew<Wt::WAnchor>("title", Wt::WLink{ Wt::LinkType::InternalPath, defaultPath }, "LMS");
        navbar->bindNew<Wt::WAnchor>("artists", Wt::WLink{ Wt::LinkType::InternalPath, "/artists" }, Wt::WString::tr("Lms.Explore.artists"));
        navbar->bindNew<Wt::WAnchor>("releases", Wt::WLink{ Wt::LinkType::InternalPath, "/releases" }, Wt::WString::tr("Lms.Explore.releases"));
        navbar->bindNew<Wt::WAnchor>("tracks", Wt::WLink{ Wt::LinkType::InternalPath, "/tracks" }, Wt::WString::tr("Lms.Explore.tracks"));
        navbar->bindNew<Wt::WAnchor>("tracklists", Wt::WLink{ Wt::LinkType::InternalPath, "/tracklists" }, Wt::WString::tr("Lms.Explore.tracklists"));

        Filters* filters{ navbar->bindNew<Filters>("filters") };
        navbar->bindString("username", std::string{ getUserLoginName() }, Wt::TextFormat::Plain);
        navbar->bindNew<Wt::WAnchor>("settings", Wt::WLink{ Wt::LinkType::InternalPath, "/settings" }, Wt::WString::tr("Lms.Settings.menu-settings"));

        {
            Wt::WAnchor* logout{ navbar->bindNew<Wt::WAnchor>("logout") };
            logout->setText(Wt::WString::tr("Lms.logout"));
            logout->clicked().connect(this, &LmsApplication::logoutUser);
        }

        if (LmsApp->getUserType() == db::UserType::ADMIN)
        {
            navbar->setCondition("if-is-admin", true);
            auto* about{ navbar->bindNew<Wt::WPushButton>("about", Wt::WString::tr("Lms.Admin.menu-about"), Wt::TextFormat::XHTML) };
            about->clicked().connect([] {
                showAboutModal();
            });
            navbar->bindNew<Wt::WAnchor>("media-libraries", Wt::WLink{ Wt::LinkType::InternalPath, "/admin/libraries" }, Wt::WString::tr("Lms.Admin.menu-media-libraries"));
            navbar->bindNew<Wt::WAnchor>("scan-settings", Wt::WLink{ Wt::LinkType::InternalPath, "/admin/scan-settings" }, Wt::WString::tr("Lms.Admin.menu-scan-settings"));
            navbar->bindNew<Wt::WAnchor>("scanner", Wt::WLink{ Wt::LinkType::InternalPath, "/admin/scanner" }, Wt::WString::tr("Lms.Admin.menu-scanner"));
            navbar->bindNew<Wt::WAnchor>("users", Wt::WLink{ Wt::LinkType::InternalPath, "/admin/users" }, Wt::WString::tr("Lms.Admin.menu-users"));
            // Hide the entry if no debug service is enabled
            if (core::Service<core::tracing::ITraceLogger>::get()
                || core::Service<db::IQueryPlanRecorder>::get())
            {
                navbar->setCondition("if-debug-tools", true);
                navbar->bindNew<Wt::WAnchor>("debug-tools", Wt::WLink{ Wt::LinkType::InternalPath, "/admin/debug-tools" }, Wt::WString::tr("Lms.Admin.menu-debug-tools"));
            }
        }

        // Contents
        // Order is important in mainStack, see IdxRoot!
        Wt::WStackedWidget* mainStack{ main->bindNew<Wt::WStackedWidget>("contents") };
        mainStack->setOverflow(Wt::Overflow::Visible); // wt makes it hidden by default

        std::unique_ptr<PlayQueue> playQueue{ std::make_unique<PlayQueue>() };
        Explore* explore{ mainStack->addNew<Explore>(*filters, *playQueue) };
        _playQueue = mainStack->addWidget(std::move(playQueue));
        mainStack->addNew<SettingsView>();

        // Admin stuff
        if (getUserType() == db::UserType::ADMIN)
        {
            mainStack->addNew<MediaLibrariesView>();
            mainStack->addNew<ScanSettingsView>();
            mainStack->addNew<ScannerController>();
            mainStack->addNew<UsersView>();
            mainStack->addNew<UserView>();
            mainStack->addNew<DebugToolsView>();
        }

        explore->getPlayQueueController().setMaxTrackCountToEnqueue(_playQueue->getCapacity());

        // Events from MediaPlayer
        _mediaPlayer->playNext.connect([this] {
            LMS_LOG(UI, DEBUG, "Received playNext from player");
            _playQueue->playNext();
        });
        _mediaPlayer->playPrevious.connect([this] {
            LMS_LOG(UI, DEBUG, "Received playPrevious from player");
            _playQueue->playPrevious();
        });

        _mediaPlayer->scrobbleListenNow.connect([this](db::TrackId trackId) {
            LMS_LOG(UI, DEBUG, "Received ScrobbleListenNow from player for trackId = " << trackId.toString());
            const scrobbling::Listen listen{ getUserId(), trackId };
            core::Service<scrobbling::IScrobblingService>::get()->listenStarted(listen);
        });
        _mediaPlayer->scrobbleListenFinished.connect([this](db::TrackId trackId, unsigned durationMs) {
            LMS_LOG(UI, DEBUG, "Received ScrobbleListenFinished from player for trackId = " << trackId.toString() << ", duration = " << (durationMs / 1000) << "s");
            const std::chrono::milliseconds duration{ durationMs };
            const scrobbling::Listen listen{ getUserId(), trackId };
            core::Service<scrobbling::IScrobblingService>::get()->listenFinished(listen, std::chrono::duration_cast<std::chrono::seconds>(duration));
        });

        _mediaPlayer->playbackEnded.connect([this] {
            LMS_LOG(UI, DEBUG, "Received playbackEnded from player");
            _playQueue->onPlaybackEnded();
        });

        _playQueue->trackSelected.connect([this](db::TrackId trackId, bool play, float replayGain) {
            _mediaPlayer->loadTrack(trackId, play, replayGain);
        });

        _playQueue->trackUnselected.connect([this] {
            _mediaPlayer->stop();
        });
        _playQueue->trackCountChanged.connect([this](std::size_t trackCount) {
            _mediaPlayer->onPlayQueueUpdated(trackCount);
        });
        _mediaPlayer->onPlayQueueUpdated(_playQueue->getCount());

        const bool isAdmin{ getUserType() == db::UserType::ADMIN };
        if (isAdmin)
        {
            _scannerEvents.scanComplete.connect([this](const scanner::ScanStats& stats) {
                notifyMsg(Notification::Type::Info,
                          Wt::WString::tr("Lms.Admin.Database.scan-complete")
                              .arg(static_cast<unsigned>(stats.getTotalFileCount()))
                              .arg(static_cast<unsigned>(stats.additions))
                              .arg(static_cast<unsigned>(stats.updates))
                              .arg(static_cast<unsigned>(stats.deletions))
                              .arg(static_cast<unsigned>(stats.duplicates.size()))
                              .arg(static_cast<unsigned>(stats.errorsCount)));
            });
        }

        internalPathChanged().connect(mainStack, [=] {
            handlePathChange(*mainStack, isAdmin);
        });

        handlePathChange(*mainStack, isAdmin);
    }

    void LmsApplication::notify(const Wt::WEvent& event)
    {
        try
        {
            LMS_SCOPED_TRACE_OVERVIEW("UI", "ProcessEvent");
            WApplication::notify(event);
        }
        catch (LmsApplicationException& e)
        {
            LMS_LOG(UI, WARNING, "Caught a LmsApplication exception: " << e.what());
            handleException(e);
        }
        catch (std::exception& e)
        {
            LMS_LOG(UI, ERROR, "Caught exception: " << e.what());
            throw core::LmsException{ "Internal error" }; // Do not put details here at it may appear on the user rendered html
        }
    }

    void LmsApplication::post(std::function<void()> func)
    {
        Wt::WServer::instance()->post(LmsApp->sessionId(), std::move(func));
    }

    void LmsApplication::setTitle(const Wt::WString& title)
    {
        if (title.empty())
            WApplication::setTitle("LMS");
        else
            WApplication::setTitle(title);
    }

    void LmsApplication::notifyMsg(Notification::Type type, const Wt::WString& message, std::chrono::milliseconds duration)
    {
        LMS_LOG(UI, INFO, "Notifying message '" << message.toUTF8() << "'");
        _notificationContainer->add(type, message, duration);
    }
} // namespace lms::ui