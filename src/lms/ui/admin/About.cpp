
#include "ModalManager.hpp"

#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>

#include "core/Version.hpp"

#include "LmsApplication.hpp"

namespace lms::ui
{
    void showAboutModal()
    {
        auto aboutModal{ std::make_unique<Wt::WTemplate>(Wt::WString::tr("Lms.Admin.About.template")) };
        Wt::WWidget* aboutModalPtr{ aboutModal.get() };
        aboutModal->addFunction("tr", &Wt::WTemplate::Functions::tr);

        aboutModal->bindString("version", std::string{ core::getVersion() }, Wt::TextFormat::Plain);
        aboutModal->bindString("homepage-link", "https://github.com/epoupon/lms");

        Wt::WPushButton* okBtn{ aboutModal->bindNew<Wt::WPushButton>("ok-btn", Wt::WString::tr("Lms.ok")) };
        okBtn->clicked().connect([=] {
            LmsApp->getModalManager().dispose(aboutModalPtr);
        });

        LmsApp->getModalManager().show(std::move(aboutModal));
    }
} // namespace lms::ui
