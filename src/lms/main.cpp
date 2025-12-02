
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
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WColor.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WServer.h>

class LmsMinimalApplication : public Wt::WApplication
{
public:
    explicit LmsMinimalApplication(const Wt::WEnvironment& env)
        : Wt::WApplication(env)
    {
        setTitle("LMS - Lightweight Music Server");

        auto container = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
        container->setStyleClass("container");

        auto layout = container->setLayout(std::make_unique<Wt::WVBoxLayout>());

        auto title = std::make_unique<Wt::WText>("<h2>LMS 复刻版已启动</h2>");
        title->setTextFormat(Wt::TextFormat::XHTML);
        layout->addWidget(std::move(title));

        auto info = std::make_unique<Wt::WText>(
            "现在这是一个最小可用的 Web 界面。<br/>"
            "接下来可以逐步接入数据库、扫描服务和完整的 UI。");
        info->setTextFormat(Wt::TextFormat::XHTML);
        layout->addWidget(std::move(info));
    }
};

static std::unique_ptr<Wt::WApplication> createLmsApp(const Wt::WEnvironment& env)
{
    return std::make_unique<LmsMinimalApplication>(env);
}

int main(int argc, char* argv[])
{
    return Wt::WRun(argc, argv, &createLmsApp);
}
