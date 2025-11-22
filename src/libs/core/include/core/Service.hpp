#pragma once

#include <cassert>
#include <memory>

namespace lms::core
{
    /**
     * @brief 服务定位器模式模板类
     * 
     * 用于全局服务管理，支持依赖注入
     * 
     * @tparam Class 服务接口类型
     * @tparam Tag 标签类型（用于区分相同接口的不同服务）
     */
    template<typename Class, typename Tag = Class>
    class Service
    {
    public:
        Service() = default;
        
        /**
         * @brief 构造函数，自动注册服务
         * @param service 服务实例
         */
        explicit Service(std::unique_ptr<Class> service)
        {
            assign(std::move(service));
        }

        ~Service()
        {
            clear();
        }

        // 禁止拷贝和移动
        Service(const Service&) = delete;
        Service(Service&&) = delete;
        Service& operator=(const Service&) = delete;
        Service& operator=(Service&&) = delete;

        /**
         * @brief 指针访问操作符
         * @return 服务指针
         */
        Class* operator->() const
        {
            return Service<Class, Tag>::get();
        }

        /**
         * @brief 解引用操作符
         * @return 服务引用
         */
        Class& operator*() const
        {
            return *Service<Class, Tag>::get();
        }

        /**
         * @brief 获取服务实例
         * @return 服务指针，如果未注册则返回 nullptr
         */
        static Class* get() 
        { 
            return _service.get(); 
        }

        /**
         * @brief 检查服务是否存在
         * @return true 如果服务已注册，false 否则
         */
        static bool exists() 
        { 
            return _service.get() != nullptr; 
        }

        /**
         * @brief 注册服务实例
         * @tparam SubClass 服务实现类类型
         * @param service 服务实例
         * @return 服务引用
         */
        template<typename SubClass>
        static Class& assign(std::unique_ptr<SubClass> service)
        {
            assert(!_service && "Service already assigned");
            _service = std::move(service);
            return *get();
        }

    private:
        /**
         * @brief 清除服务实例
         */
        static void clear() 
        { 
            _service.reset(); 
        }

        // 静态服务实例（每个模板特化一个实例）
        static inline std::unique_ptr<Class> _service;
    };
} // namespace lms::core

