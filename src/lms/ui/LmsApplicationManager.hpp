
#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include <Wt/WSignal.h>

#include "database/objects/UserId.hpp"

namespace lms::ui
{
    class LmsApplication;
    class LmsApplicationManager
    {
    public:
        Wt::Signal<LmsApplication&> applicationRegistered;
        Wt::Signal<LmsApplication&> applicationUnregistered;

    private:
        friend class LmsApplication;

        void registerApplication(LmsApplication& application);
        void unregisterApplication(LmsApplication& application);

        std::mutex _mutex;
        std::unordered_map<db::UserId, std::unordered_set<LmsApplication*>> m_applications;
    };
} // namespace lms::ui
