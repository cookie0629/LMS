
#pragma once

#include "RefreshStep.hpp"

namespace lms::podcast
{
    class ClearTmpDirectoryStep : public RefreshStep
    {
    public:
        using RefreshStep::RefreshStep;

    private:
        core::LiteralString getName() const override;
        void run() override;
    };

} // namespace lms::podcast