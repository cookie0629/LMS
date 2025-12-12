
#pragma once

#include <Wt/WValidator.h>

namespace lms::ui
{
    std::unique_ptr<Wt::WValidator> createLoginNameValidator();
} // namespace lms::ui
