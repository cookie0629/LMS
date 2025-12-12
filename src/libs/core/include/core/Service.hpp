// 服务基类与生命周期管理

#pragma once

#include <cassert>
#include <memory>

namespace lms::core
{
    // Tag can be used if you have multiple services sharing the same interface
    template<typename Class, typename Tag = Class>
    class Service
    {
    public:
        Service() = default;
        Service(std::unique_ptr<Class> service)
        {
            assign(std::move(service));
        }

        ~Service()
        {
            clear();
        }

        Service(const Service&) = delete;
        Service(Service&&) = delete;
        Service& operator=(const Service&) = delete;
        Service& operator=(Service&&) = delete;

        Class* operator->() const
        {
            return Service<Class, Tag>::get();
        }

        Class& operator*() const
        {
            return *Service<Class, Tag>::get();
        }

        static Class* get() { return _service.get(); }
        static bool exists() { return _service.get(); }

        template<typename SubClass>
        static Class& assign(std::unique_ptr<SubClass> service)
        {
            assert(!_service);
            _service = std::move(service);
            return *get();
        }

    private:
        static void clear() { _service.reset(); }

        static inline std::unique_ptr<Class> _service;
    };
} // namespace lms::core