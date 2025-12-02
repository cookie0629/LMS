
/*
 * Minimal Wt-based LMS entry point: start an HTTP server and show a simple page.
 * 后续可以逐步替换为完整的模板版 UI 和服务初始化逻辑。
 */

#include <memory>
#include <filesystem>
#include <iostream>

#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WServer.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WBreak.h>
#include <Wt/WString.h>

#include <Wt/Dbo/collection.h>

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Artist.hpp"
#include "services/scanner/IScannerService.hpp"

namespace
{
    void initBackend()
    {
        namespace fs = std::filesystem;

        // 1. 加载配置
        const fs::path configPath{ "../conf/lms.conf" };
        auto config = lms::core::createConfig(configPath);
        lms::core::Service<lms::core::IConfig>::assign(std::move(config));

        // 2. 初始化日志系统
        auto logger = lms::core::logging::createLogger();
        lms::core::Service<lms::core::logging::ILogger>::assign(std::move(logger));

        // 3. 工作目录与缓存目录
        fs::path workingDir{ "/var/lms" };
        if (auto* cfg = lms::core::Service<lms::core::IConfig>::get())
        {
            workingDir = cfg->getPath("working-dir", workingDir);
        }

        fs::create_directories(workingDir);
        fs::create_directories(workingDir / "cache");

        // 4. 创建数据库并准备表结构
        const fs::path dbPath{ workingDir / "lms.db" };
        const bool dbExistsBefore = fs::exists(dbPath);

        auto db = lms::db::createDb(dbPath, 10);
        auto& dbRef = lms::core::Service<lms::db::IDb>::assign(std::move(db));

        lms::db::Session& session{ dbRef.getTLSSession() };
        if (!dbExistsBefore)
        {
            // 仅在数据库首次创建时准备表和迁移
            session.prepareTablesIfNeeded();
            {
                auto tx = session.createWriteTransaction();
                session.migrateSchemaIfNeeded();
            }
        }

        // 5. 创建扫描服务（后续用于真正的媒体库扫描）
        auto scanner = lms::scanner::createScannerService(dbRef, workingDir / "cache");
        lms::core::Service<lms::scanner::IScannerService>::assign(std::move(scanner));
    }
} // namespace

class LmsMinimalApplication : public Wt::WApplication
{
public:
    explicit LmsMinimalApplication(const Wt::WEnvironment& env)
        : Wt::WApplication(env)
        , _loggedIn(false)
    {
        setTitle("LMS - Lightweight Music Server");

        setTheme(std::make_shared<Wt::WBootstrapTheme>());

        showLogin();
    }

private:
    void showLogin()
    {
        root()->clear();

        auto container = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto layout = container->setLayout(std::make_unique<Wt::WVBoxLayout>());

        auto title = std::make_unique<Wt::WText>("<h2>LMS 登录</h2>");
        title->setTextFormat(Wt::TextFormat::XHTML);
        layout->addWidget(std::move(title));

        auto info = std::make_unique<Wt::WText>("示例账号：用户名 admin / 密码 adminadmin（临时实现）");
        info->setTextFormat(Wt::TextFormat::Plain);
        layout->addWidget(std::move(info));

        auto userEdit = layout->addWidget(std::make_unique<Wt::WLineEdit>());
        userEdit->setPlaceholderText("用户名");

        auto passEdit = layout->addWidget(std::make_unique<Wt::WLineEdit>());
        passEdit->setPlaceholderText("密码");
        passEdit->setEchoMode(Wt::EchoMode::Password);

        layout->addWidget(std::make_unique<Wt::WBreak>());

        auto errorText = layout->addWidget(std::make_unique<Wt::WText>(""));
        errorText->setStyleClass("text-danger");

        auto loginButton = layout->addWidget(std::make_unique<Wt::WPushButton>("登录"));

        loginButton->clicked().connect([this, userEdit, passEdit, errorText] {
            const auto user = userEdit->text().toUTF8();
            const auto pass = passEdit->text().toUTF8();

            if (user == "admin" && pass == "adminadmin")
            {
                _loggedIn = true;
                showMediaBrowser();
            }
            else
            {
                errorText->setText("用户名或密码错误");
            }
        });
    }

    void showMediaBrowser()
    {
        root()->clear();

        auto container = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto layout = container->setLayout(std::make_unique<Wt::WVBoxLayout>());

        auto title = std::make_unique<Wt::WText>("<h2>LMS 媒体浏览</h2>");
        title->setTextFormat(Wt::TextFormat::XHTML);
        layout->addWidget(std::move(title));

        // 显示数据库中的艺术家数量
        std::string dbSummary;
        std::string artistsListHtml;
        if (auto* db = lms::core::Service<lms::db::IDb>::get())
        {
            try
            {
                lms::db::Session& session{ db->getTLSSession() };
                auto tx = session.createReadTransaction();
                const std::size_t artistCount = lms::db::Artist::getCount(session);
                dbSummary = "当前数据库中的艺术家数量: " + std::to_string(artistCount);

                // 读取前 20 个艺术家名称
                Wt::Dbo::collection<Wt::Dbo::ptr<lms::db::Artist>> artists =
                    session.getDboSession()->find<lms::db::Artist>().orderBy("sort_name").limit(20).resultList();
                artistsListHtml = "<ul>";
                for (const auto& artistPtr : artists)
                {
                    artistsListHtml += "<li>";
                    std::string nameStr{ artistPtr->getName() };
                    artistsListHtml += Wt::WString::fromUTF8(nameStr).toUTF8();
                    artistsListHtml += "</li>";
                }
                artistsListHtml += "</ul>";
            }
            catch (const std::exception& e)
            {
                dbSummary = std::string{ "读取数据库时出错: " } + e.what();
            }
        }
        else
        {
            dbSummary = "数据库服务尚未初始化。";
        }

        auto info = std::make_unique<Wt::WText>(
            "登录已成功。下面是从真实数据库读取的一些信息：<br/>" + dbSummary + "<br/>");
        info->setTextFormat(Wt::TextFormat::XHTML);
        layout->addWidget(std::move(info));

        auto artistsTitle = std::make_unique<Wt::WText>("<h4>部分艺术家列表（最多 20 个）</h4>");
        artistsTitle->setTextFormat(Wt::TextFormat::XHTML);
        layout->addWidget(std::move(artistsTitle));

        auto artistsText = std::make_unique<Wt::WText>(artistsListHtml.empty() ? "<i>当前暂无艺术家，请先配置媒体库并执行扫描。</i>" : artistsListHtml);
        artistsText->setTextFormat(Wt::TextFormat::XHTML);
        layout->addWidget(std::move(artistsText));

        // 扫描按钮
        auto scanStatus = layout->addWidget(std::make_unique<Wt::WText>(""));
        auto scanBtn = layout->addWidget(std::make_unique<Wt::WPushButton>("立即扫描媒体库"));
        scanBtn->clicked().connect([scanStatus] {
            if (auto* scanner = lms::core::Service<lms::scanner::IScannerService>::get())
            {
                lms::scanner::ScanOptions opts;
                opts.fullScan = true;
                scanner->requestImmediateScan(opts);
                scanStatus->setText("已请求扫描，稍后刷新页面查看更新结果。");
            }
            else
            {
                scanStatus->setText("扫描服务尚未初始化。");
            }
        });

        auto logoutBtn = layout->addWidget(std::make_unique<Wt::WPushButton>("退出登录"));
        logoutBtn->clicked().connect([this] {
            _loggedIn = false;
            showLogin();
        });
    }

    bool _loggedIn;
};

static std::unique_ptr<Wt::WApplication> createLmsApp(const Wt::WEnvironment& env)
{
    return std::make_unique<LmsMinimalApplication>(env);
}

int main(int argc, char* argv[])
{
    try
    {
        initBackend();
    }
    catch (const std::exception& e)
    {
        std::cerr << "后端初始化失败: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return Wt::WRun(argc, argv, &createLmsApp);
}
