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

#include <thread>

#include <Wt/WApplication.h>
#include <Wt/WLogSink.h>
#include <Wt/WServer.h>
#include <boost/asio/io_context.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "core/IChildProcessManager.hpp"
#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/IOContextRunner.hpp"
#include "core/ITraceLogger.hpp"
#include "core/Service.hpp"
#include "core/String.hpp"
#include "core/SystemPaths.hpp"
#include "database/IDb.hpp"
#include "database/IQueryPlanRecorder.hpp"
#include "database/Session.hpp"
#include "image/Image.hpp"
#include "services/artwork/IArtworkService.hpp"
#include "services/auth/IAuthTokenService.hpp"
#include "services/auth/IEnvService.hpp"
#include "services/auth/IPasswordService.hpp"
#include "services/feedback/IFeedbackService.hpp"
#include "services/podcast/IPodcastService.hpp"
#include "services/recommendation/IPlaylistGeneratorService.hpp"
#include "services/recommendation/IRecommendationService.hpp"
#include "services/scanner/IScannerService.hpp"
#include "services/scrobbling/IScrobblingService.hpp"
#include "services/transcoding/ITranscodeService.hpp"
#include "subsonic/SubsonicResource.hpp"
#include "ui/Auth.hpp"
#include "ui/LmsApplication.hpp"
#include "ui/LmsApplicationManager.hpp"
#include "ui/LmsInitApplication.hpp"

namespace lms
{
    namespace
    {
        // Compute how many threads should be used for background work (Wt HTTP server, IO, DB, etc.).
        // 计算后台工作线程数量（Wt HTTP 服务器、IO、数据库等）。
        // Вычисляет количество потоков для фоновых задач (Wt HTTP сервер, IO, БД и т.д.).
        std::size_t getThreadCount()
        {
            const unsigned long configHttpServerThreadCount{ core::Service<core::IConfig>::get()->getULong("http-server-thread-count", 0) };

            // Reserve at least 2 threads since we still have some blocking IO (for example when reading from ffmpeg)
            // 保留至少 2 个线程，因为我们仍有一些阻塞 IO（例如从 ffmpeg 读取时）
            // Резервируем минимум 2 потока, так как у нас всё ещё есть блокирующий IO (например, при чтении из ffmpeg)
            return configHttpServerThreadCount ? configHttpServerThreadCount : std::max<unsigned long>(2, std::thread::hardware_concurrency());
        }

        // Read UI authentication backend from config ("internal" / "pam" / "http-headers").
        // 从配置中读取 UI 认证后端（"internal" / "pam" / "http-headers"）。
        // 这个值直接影响登录方式：
        // - internal: 内置用户名/密码；
        // - pam:      使用系统 PAM 认证；
        // - http-headers: 信任反向代理注入的 HTTP 头。
        // Читает бэкенд аутентификации UI из конфига ("internal" / "pam" / "http-headers").
        // Это значение напрямую влияет на способ входа:
        // - internal: встроенная аутентификация по имени/паролю;
        // - pam:      использование системной PAM-аутентификации;
        // - http-headers: доверие HTTP-заголовкам, внедрённым обратным прокси.
        ui::AuthenticationBackend getUIAuthenticationBackend()
        {
            const std::string backend{ core::stringUtils::stringToLower(core::Service<core::IConfig>::get()->getString("authentication-backend", "internal")) };
            if (backend == "internal")
                return ui::AuthenticationBackend::Internal;
            if (backend == "pam")
                return ui::AuthenticationBackend::PAM;
            if (backend == "http-headers")
                return ui::AuthenticationBackend::Env;

            throw core::LmsException{ "Invalid config value for 'authentication-backend'" };
        }

        // Read tracing level for detailed performance tracing ("disabled" / "overview" / "detailed").
        // 从配置中读取 tracing 级别：用于性能/行为跟踪（禁用 / 概览 / 详细）。
        // Читает уровень трассировки из конфига: для отслеживания производительности/поведения (отключено / обзор / детально).
        std::optional<core::tracing::Level> getTracingLevel()
        {
            std::string_view tracingLevel{ core::Service<core::IConfig>::get()->getString("tracing-level", "disabled") };

            if (tracingLevel == "disabled")
                return std::nullopt;
            else if (tracingLevel == "overview")
                return core::tracing::Level::Overview;
            else if (tracingLevel == "detailed")
                return core::tracing::Level::Detailed;

            throw core::LmsException{ "Invalid config value for 'tracing-level'" };
        }

        // Build command‑line arguments for Wt::WServer and generate wt_config.xml file.
        // 为 Wt::WServer 生成命令行参数，并写出 wt_config.xml 配置文件。
        // 演示功能：你在日志里看到的一串 ARG=... 就是这里生成的。
        // Строит аргументы командной строки для Wt::WServer и генерирует файл wt_config.xml.
        // Примечание: строка ARG=... в логах генерируется здесь.
        std::vector<std::string> generateWtConfig(std::string execPath)
        {
            core::IConfig& config{ *core::Service<core::IConfig>::get() };

            std::vector<std::string> args;

            const std::filesystem::path wtConfigPath{ config.getPath("working-dir", "/var/lms") / "wt_config.xml" };
            const std::filesystem::path wtResourcesPath{ config.getPath("wt-resources", "/usr/share/Wt/resources") };

            args.push_back(execPath);
            args.push_back("--config=" + wtConfigPath.string());
            args.push_back("--docroot=" + std::string{ config.getString("docroot", "/usr/share/lms/docroot/;/resources,/css,/images,/js,/favicon.ico") });
            args.push_back("--approot=" + std::string{ config.getString("approot", "/usr/share/lms/approot") });
            args.push_back("--deploy-path=" + std::string{ config.getString("deploy-path", "/") });
            if (!wtResourcesPath.empty())
                args.push_back("--resources-dir=" + wtResourcesPath.string());

            if (core::Service<core::IConfig>::get()->getBool("tls-enable", false))
            {
                args.push_back("--https-port=" + std::to_string(config.getULong("listen-port", 5082)));
                args.push_back("--https-address=" + std::string{ config.getString("listen-addr", "0.0.0.0") });
                args.push_back("--ssl-certificate=" + std::string{ config.getString("tls-cert", "/var/lms/cert.pem") });
                args.push_back("--ssl-private-key=" + std::string{ config.getString("tls-key", "/var/lms/privkey.pem") });
                args.push_back("--ssl-tmp-dh=" + std::string{ config.getString("tls-dh", "/var/lms/dh2048.pem") });
            }
            else
            {
                args.push_back("--http-port=" + std::to_string(config.getULong("listen-port", 5082)));
                args.push_back("--http-address=" + std::string{ config.getString("listen-addr", "0.0.0.0") });
            }

            args.push_back("--threads=" + std::to_string(getThreadCount()));

            // Generate the wt_config.xml file
            boost::property_tree::ptree pt;

            pt.put("server.application-settings.<xmlattr>.location", "*");

            // Reverse proxy
            if (config.getBool("behind-reverse-proxy", false))
            {
                pt.put("server.application-settings.trusted-proxy-config.original-ip-header", config.getString("original-ip-header", "X-Forwarded-For"));
                config.visitStrings("trusted-proxies", [&](std::string_view trustedProxy) {
                    pt.add("server.application-settings.trusted-proxy-config.trusted-proxies.proxy", std::string{ trustedProxy });
                },
                                    { "127.0.0.1", "::1" });
            }

            {
                boost::property_tree::ptree viewport;
                viewport.put("<xmlattr>.name", "viewport");
                viewport.put("<xmlattr>.content", "width=device-width, initial-scale=1, user-scalable=no");
                pt.add_child("server.application-settings.head-matter.meta", viewport);
            }
            {
                boost::property_tree::ptree themeColor;
                themeColor.put("<xmlattr>.name", "theme-color");
                themeColor.put("<xmlattr>.content", "#303030");
                pt.add_child("server.application-settings.head-matter.meta", themeColor);
            }

            {
                std::ofstream oss{ wtConfigPath, std::ios::out };
                if (!oss)
                    throw core::LmsException{ "Can't open '" + wtConfigPath.string() + "' for writing!" };

                boost::property_tree::xml_parser::write_xml(oss, pt);

                if (!oss)
                    throw core::LmsException{ "Can't write in file '" + wtConfigPath.string() + "', no space left?" };
            }

            return args;
        }

        // proxyScannerEventsToApplication: 将扫描器服务的事件转发到所有活动的 Web 应用会话。
        // 当扫描器状态改变时（开始、完成、进度更新等），所有已登录的用户界面都会收到通知。
        // proxyScannerEventsToApplication: перенаправляет события сервиса сканера во все активные сессии веб-приложения.
        // При изменении состояния сканера (начало, завершение, обновление прогресса и т.д.) все залогиненные UI получают уведомления.
        void proxyScannerEventsToApplication(scanner::IScannerService& scanner, Wt::WServer& server)
        {
            auto postAll{ [](Wt::WServer& server, std::function<void()> cb) {
                server.postAll([cb = std::move(cb)] {
                    // may be nullptr, see https://redmine.webtoolkit.eu/issues/8202
                    // 可能为 nullptr，参见 https://redmine.webtoolkit.eu/issues/8202
                    // может быть nullptr, см. https://redmine.webtoolkit.eu/issues/8202
                    if (LmsApp)
                        cb();
                });
            } };

            scanner.getEvents().scanAborted.connect([&] {
                postAll(server, [] {
                    LmsApp->getScannerEvents().scanAborted.emit();
                    LmsApp->triggerUpdate();
                });
            });

            scanner.getEvents().scanStarted.connect([&] {
                postAll(server, [] {
                    LmsApp->getScannerEvents().scanStarted.emit();
                    LmsApp->triggerUpdate();
                });
            });

            scanner.getEvents().scanComplete.connect([&](const scanner::ScanStats& stats) {
                postAll(server, [=] {
                    LmsApp->getScannerEvents().scanComplete.emit(stats);
                    LmsApp->triggerUpdate();
                });
            });

            scanner.getEvents().scanInProgress.connect([&](const scanner::ScanStepStats& stats) {
                postAll(server, [=] {
                    LmsApp->getScannerEvents().scanInProgress.emit(stats);
                    LmsApp->triggerUpdate();
                });
            });

            scanner.getEvents().scanScheduled.connect([&](const Wt::WDateTime dateTime) {
                postAll(server, [=] {
                    LmsApp->getScannerEvents().scanScheduled.emit(dateTime);
                    LmsApp->triggerUpdate();
                });
            });
        }

        // getLogMinSeverity: 从配置中读取最小日志级别，用于过滤日志输出。
        // getLogMinSeverity: читает минимальный уровень логирования из конфига для фильтрации вывода логов.
        core::logging::Severity getLogMinSeverity()
        {
            std::string_view minSeverity{ core::Service<core::IConfig>::get()->getString("log-min-severity", "info") };

            if (minSeverity == "debug")
                return core::logging::Severity::DEBUG;
            else if (minSeverity == "info")
                return core::logging::Severity::INFO;
            else if (minSeverity == "warning")
                return core::logging::Severity::WARNING;
            else if (minSeverity == "error")
                return core::logging::Severity::ERROR;
            else if (minSeverity == "fatal")
                return core::logging::Severity::FATAL;

            throw core::LmsException{ "Invalid config value for 'log-min-severity'" };
        }

        // LmsLogSink: 将 Wt 框架的日志输出重定向到 LMS 的日志系统。
        // LmsLogSink: перенаправляет логи фреймворка Wt в систему логирования LMS.
        class LmsLogSink : public Wt::WLogSink
        {
        public:
            LmsLogSink(core::logging::ILogger& logger)
                : _logger{ logger }
            {
            }

        private:
            void log(const std::string& type, const std::string& scope, const std::string& message) const noexcept override
            {
                // Some wt code path may go here without testing logging()
                // 某些 Wt 代码路径可能不检查 logging() 就调用这里
                // некоторые пути кода Wt могут вызвать это без проверки logging()
                if (logging(type, scope))
                {
                    const core::logging::Severity severity{ getSeverity(type, scope) };
                    _logger.processLog(core::logging::Module::WT, severity, message);
                }
            }

            bool logging(const std::string& type, const std::string& scope) const noexcept override
            {
                const core::logging::Severity severity{ getSeverity(type, scope) };
                return _logger.isSeverityActive(severity);
            }

            static core::logging::Severity getSeverity(const std::string& type, const std::string& scope)
            {
                return adjustSeverity(getSeverityFromString(type), scope);
            }

            static core::logging::Severity adjustSeverity(core::logging::Severity initialSeverity, std::string_view scope)
            {
                if (initialSeverity == core::logging::Severity::INFO && (scope == "WebRequest" || scope == "wthttp"))
                    return core::logging::Severity::DEBUG;

                return initialSeverity;
            }

            static core::logging::Severity getSeverityFromString(std::string_view type)
            {
                if (type == "debug")
                    return core::logging::Severity::DEBUG;
                if (type == "info")
                    return core::logging::Severity::INFO;
                if (type == "warning")
                    return core::logging::Severity::WARNING;
                if (type == "error")
                    return core::logging::Severity::ERROR;
                if (type == "fatal")
                    return core::logging::Severity::FATAL;

                return core::logging::Severity::INFO;
            }

            core::logging::ILogger& _logger;
        };

    } // namespace

    // main: LMS 服务器的主入口点，负责初始化所有服务、数据库、Web 服务器并启动事件循环。
    // main: главная точка входа сервера LMS, инициализирует все сервисы, БД, веб-сервер и запускает цикл событий.
    int main(int argc, char* argv[])
    {
        std::filesystem::path configFilePath{ core::sysconfDirectory / "lms.conf" };
        int res{ EXIT_FAILURE };

        assert(argc > 0);
        assert(argv[0] != NULL);

        // displayUsage: 显示命令行用法帮助信息。
        // displayUsage: показывает справку по использованию командной строки.
        auto displayUsage{ [&](std::ostream& os) {
            os << "Usage:\t" << argv[0] << "\t[conf_file]\n\n"
               << "Options:\n"
               << "\tconf_file:\t path to the LMS configuration file (defaults to " << configFilePath << ")\n\n";
        } };

        if (argc == 2)
        {
            const std::string_view arg{ argv[1] };
            if (arg == "-h" || arg == "--help")
            {
                displayUsage(std::cout);
                return EXIT_SUCCESS;
            }
            configFilePath = std::string(arg, 0, 256);
        }
        else if (argc > 2)
        {
            displayUsage(std::cerr);
            return EXIT_FAILURE;
        }

        try
        {
            // 关闭标准输入，因为服务器不需要交互式输入
            // Закрываем стандартный ввод, так как серверу не нужен интерактивный ввод
            close(STDIN_FILENO);

            // 初始化核心服务：配置、日志、追踪
            // Инициализация основных сервисов: конфигурация, логирование, трассировка
            core::Service<core::IConfig> config{ core::createConfig(configFilePath) };
            core::Service<core::logging::ILogger> logger{ createLogger(getLogMinSeverity(), config->getPath("log-file", "")) };
            core::Service<core::tracing::ITraceLogger> traceLogger;
            if (const auto level{ getTracingLevel() })
                traceLogger.assign(core::tracing::createTraceLogger(level.value(), config->getULong("tracing-buffer-size", core::tracing::MinBufferSizeInMBytes)));

            // use system locale. libarchive relies on this to write filenames
            // 使用系统区域设置。libarchive 依赖此来写入文件名
            // Используем системную локаль. libarchive полагается на это для записи имён файлов
            if (char* locale{ ::setlocale(LC_ALL, "") })
                LMS_LOG(MAIN, INFO, "locale set to '" << locale << "'");
            else
                LMS_LOG(MAIN, WARNING, "Cannot set locale from system");

            // Make sure the working directory exists
            // 确保工作目录存在（用于数据库、缓存等）
            // Убеждаемся, что рабочий каталог существует (для БД, кэша и т.д.)
            const std::filesystem::path workingDirectoryPath{ config->getPath("working-dir", "/var/lms") };
            const std::filesystem::path cachePath{ workingDirectoryPath / "cache" };
            std::filesystem::create_directories(workingDirectoryPath);
            std::filesystem::create_directories(cachePath);

            // Construct WT configuration and get the argc/argv back
            // 构建 Wt 配置并获取 argc/argv 参数
            // Строим конфигурацию Wt и получаем аргументы argc/argv
            const std::vector<std::string> wtServerArgs{ generateWtConfig(argv[0]) };

            std::vector<const char*> wtArgv(wtServerArgs.size());
            for (std::size_t i = 0; i < wtServerArgs.size(); ++i)
            {
                std::cout << "ARG = " << wtServerArgs[i] << std::endl;
                wtArgv[i] = wtServerArgs[i].c_str();
            }

            // 设置 Wt 服务器：自定义日志、配置参数
            // Настройка сервера Wt: пользовательский логгер, параметры конфигурации
            LmsLogSink lmsLogSink{ *logger };
            Wt::WServer server{ argv[0] };
            server.setCustomLogger(lmsLogSink);
            server.setServerConfiguration(wtServerArgs.size(), const_cast<char**>(&wtArgv[0]));

            // As initialization can take a while (db migration, analyze, etc.), we bind a temporary init entry point to warn the user
            // 由于初始化可能需要较长时间（数据库迁移、分析等），我们绑定一个临时的初始化入口点来提示用户
            // Так как инициализация может занять время (миграция БД, анализ и т.д.), привязываем временную точку входа для предупреждения пользователя
            server.addEntryPoint(Wt::EntryPointType::Application,
                                 [&](const Wt::WEnvironment& env) {
                                     return ui::LmsInitApplication::create(env);
                                 });

            LMS_LOG(MAIN, INFO, "Starting init web server...");
            server.start();

            // ioContext used to dispatch all the services that are out of the Wt event loop
            // ioContext 用于调度所有在 Wt 事件循环之外的服务
            // ioContext используется для диспетчеризации всех сервисов вне цикла событий Wt
            boost::asio::io_context ioContext;
            core::IOContextRunner ioContextRunner{ ioContext, getThreadCount(), "Misc" };

            // 查询计划记录器（可选，用于性能分析）
            // Регистратор планов запросов (опционально, для анализа производительности)
            core::Service<db::IQueryPlanRecorder> queryPlanRecorder;
            if (config->getBool("db-record-query-plans", false))
                queryPlanRecorder.assign(db::createQueryPlanRecorder());

            // Connection pool size must be twice the number of threads: we have at least 2 io pools with getThreadCount() each and they all may access the database
            // 连接池大小必须是线程数的两倍：我们至少有 2 个 IO 池，每个有 getThreadCount() 个线程，它们都可能访问数据库
            // Размер пула соединений должен быть в два раза больше числа потоков: у нас минимум 2 IO-пула по getThreadCount() потоков, и все они могут обращаться к БД
            auto database{ db::createDb(config->getPath("working-dir", "/var/lms") / "lms.db", getThreadCount() * 2) };
            {
                db::Session session{ *database };
                session.prepareTablesIfNeeded();
                bool migrationPerformed{ session.migrateSchemaIfNeeded() };
                session.createIndexesIfNeeded();

                // As this may be quite long, we only do it during startup
                // 由于这可能耗时较长，我们只在启动时执行
                // Так как это может занять время, выполняем только при запуске
                if (migrationPerformed)
                    session.vacuum();
                else
                    session.vacuumIfNeeded();
            }

            ui::LmsApplicationManager appManager;

            const std::size_t loginThrottlerMaxEntries{ config->getULong("login-throttler-max-entries", 10'000) };
            // Service initialization order is important (reverse-order for deinit)
            // 服务初始化顺序很重要（析构时按相反顺序）
            // Порядок инициализации сервисов важен (при деинициализации — обратный порядок)
            core::Service<core::IChildProcessManager> childProcessManagerService{ core::createChildProcessManager(ioContext) };

            const ui::AuthenticationBackend uiAuthenticationBackend{ getUIAuthenticationBackend() };
            core::Service<auth::IAuthTokenService> authTokenService{ auth::createAuthTokenService(*database, config->getULong("login-throttler-max-entriees", 10'000)) };
            core::Service<auth::IPasswordService> authPasswordService;
            core::Service<auth::IEnvService> authEnvService;

            // 注册 UI 域的认证令牌参数：单次使用，有效期 8 周
            // Регистрируем параметры токенов аутентификации для домена UI: одноразовое использование, срок действия 8 недель
            authTokenService->registerDomain("ui", auth::IAuthTokenService::DomainParameters{
                                                       .tokenMaxUseCount = 1,
                                                       .tokenDuration = std::chrono::weeks{ 8 },
                                                   });

            // 注册 Subsonic API 域的认证令牌参数：无使用次数限制，无时间限制
            // Регистрируем параметры токенов для домена Subsonic API: без ограничения использования, без ограничения времени
            authTokenService->registerDomain("subsonic", auth::IAuthTokenService::DomainParameters{
                                                             .tokenMaxUseCount = std::nullopt, // no usage limit
                                                             .tokenDuration = std::nullopt,    // no time limit
                                                         });

            switch (uiAuthenticationBackend)
            {
            case ui::AuthenticationBackend::Internal:
                authPasswordService.assign(auth::createPasswordService("internal", *database, loginThrottlerMaxEntries));
                break;
            case ui::AuthenticationBackend::PAM:
                authPasswordService.assign(auth::createPasswordService("PAM", *database, loginThrottlerMaxEntries));
                break;
            case ui::AuthenticationBackend::Env:
                authEnvService.assign(auth::createEnvService("http-headers", *database));
                break;
            }

            image::init(argv[0]);
            core::Service<artwork::IArtworkService> artworkService{ artwork::createArtworkService(*database, server.appRoot() + "/images/unknown-cover.svg", server.appRoot() + "/images/unknown-artist.svg") };
            core::Service<recommendation::IRecommendationService> recommendationService{ recommendation::createRecommendationService(*database) };
            core::Service<recommendation::IPlaylistGeneratorService> playlistGeneratorService{ recommendation::createPlaylistGeneratorService(*database, *recommendationService) };
            core::Service<scanner::IScannerService> scannerService{ scanner::createScannerService(*database, cachePath) };
            core::Service<transcoding::ITranscodeService> transcodingService{ transcoding::createTranscodeService(*database, *childProcessManagerService) };
            core::Service<podcast::IPodcastService> podcastService{ podcast::createPodcastService(ioContext, *database, cachePath / "podcasts") };

            // 扫描完成时刷新封面缓存（即使没有变更）
            // 封面可能是外部文件，可能已更改但我们目前不跟踪它们（但应该跟踪）
            // При завершении сканирования сбрасываем кэш обложек (даже если изменений нет)
            // Обложки могут быть внешними файлами, которые изменились, но мы пока не отслеживаем их (но должны)
            scannerService->getEvents().scanComplete.connect([&] {
                // Flush cover cache even if no changes:
                // covers may be external files that changed and we don't keep track of them for now (but we should)
                artworkService->flushCache();
            });

            core::Service<feedback::IFeedbackService> feedbackService{ feedback::createFeedbackService(ioContext, *database) };
            core::Service<scrobbling::IScrobblingService> scrobblingService{ scrobbling::createScrobblingService(ioContext, *database) };

            LMS_LOG(MAIN, INFO, "Stopping init web server...");
            server.stop();

            server.removeEntryPoint("");

            std::unique_ptr<Wt::WResource> subsonicResource;
            // bind API resources
            // 绑定 API 资源（Subsonic 兼容 API）
            // Привязываем ресурсы API (совместимый с Subsonic API)
            if (config->getBool("api-subsonic", true))
            {
                subsonicResource = api::subsonic::createSubsonicResource(*database);
                server.addResource(subsonicResource.get(), "/rest");
            }

            // bind UI entry point
            // 绑定 UI 入口点（主 Web 应用）
            // Привязываем точку входа UI (основное веб-приложение)
            server.addEntryPoint(Wt::EntryPointType::Application,
                                 [&database, &appManager, uiAuthenticationBackend](const Wt::WEnvironment& env) {
                                     return ui::LmsApplication::create(env, *database, appManager, uiAuthenticationBackend);
                                 });

            proxyScannerEventsToApplication(*scannerService, server);

            LMS_LOG(MAIN, INFO, "Starting init web server...");
            server.start();

            LMS_LOG(MAIN, INFO, "Now running...");
            Wt::WServer::waitForShutdown();

            LMS_LOG(MAIN, INFO, "Stopping server...");
            server.stop();

            LMS_LOG(MAIN, INFO, "Quitting...");
            res = EXIT_SUCCESS;
        }
        catch (const Wt::WServer::Exception& e)
        {
            LMS_LOG(MAIN, FATAL, "Caught WServer::Exception: " << e.what());
            std::cerr << "Caught a WServer::Exception: " << e.what() << std::endl;
            res = EXIT_FAILURE;
        }
        catch (const std::exception& e)
        {
            LMS_LOG(MAIN, FATAL, "Caught std::exception: " << e.what());
            std::cerr << "Caught std::exception: " << e.what() << std::endl;
            res = EXIT_FAILURE;
        }

        return res;
    }
} // namespace lms

int main(int argc, char* argv[])
{
    return lms::main(argc, argv);
}
