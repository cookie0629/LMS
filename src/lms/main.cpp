
/*
 * Minimal Wt-based LMS entry point: start an HTTP server and show a simple page.
 * 后续可以逐步替换为完整的模板版 UI 和服务初始化逻辑。
 */

#include <memory>

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

        auto title = std::make_unique<Wt::WText>("<h2>LMS 媒体浏览（占位实现）</h2>");
        title->setTextFormat(Wt::TextFormat::XHTML);
        layout->addWidget(std::move(title));

        auto info = std::make_unique<Wt::WText>(
            "登录已成功。本页面目前只是占位的媒体浏览界面，<br/>"
            "接下来会接入真实的数据库和扫描服务，展示你的艺术家 / 专辑 / 曲目。");
        info->setTextFormat(Wt::TextFormat::XHTML);
        layout->addWidget(std::move(info));

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
    return Wt::WRun(argc, argv, &createLmsApp);
}
