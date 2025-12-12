
#pragma once

#include <Wt/WValidator.h>

namespace lms::ui
{
    std::unique_ptr<Wt::WValidator> createMandatoryValidator();
} // namespace lms::ui
