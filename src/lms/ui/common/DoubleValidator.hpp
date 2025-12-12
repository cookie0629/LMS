
#pragma once

#include <Wt/WValidator.h>

namespace lms::ui
{
    std::unique_ptr<Wt::WValidator> createDoubleValidator(double min, double max);
} // namespace lms::ui
