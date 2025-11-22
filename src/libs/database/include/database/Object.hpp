#pragma once

#include <Wt/Dbo/ptr.h>

#include "database/IdType.hpp"

namespace lms::db
{
    /**
     * @brief 对象指针基类
     */
    class ObjectPtrBase
    {
    protected:
        static void checkWriteTransaction(Wt::Dbo::Session& session);
    };

    /**
     * @brief 对象指针模板类
     */
    template<typename T>
    class ObjectPtr : public ObjectPtrBase
    {
    public:
        ObjectPtr() = default;
        ObjectPtr(const Wt::Dbo::ptr<T>& obj) : _obj{ obj } {}
        ObjectPtr(Wt::Dbo::ptr<T>&& obj) : _obj{ std::move(obj) } {}

        const T* operator->() const { return _obj.get(); }
        T* operator->() { return _obj.get(); }
        operator bool() const { return _obj.get(); }
        bool operator!() const { return !_obj.get(); }
        bool operator==(const ObjectPtr& other) const { return _obj == other._obj; }
        bool operator!=(const ObjectPtr& other) const { return _obj != other._obj; }

        auto modify()
        {
            checkWriteTransaction(*_obj.session());
            return _obj.modify();
        }

        void remove()
        {
            checkWriteTransaction(*_obj.session());
            _obj.remove();
        }

    private:
        template<typename, typename>
        friend class Object;
        Wt::Dbo::ptr<T> _obj;
    };

    /**
     * @brief 数据库对象基类模板
     * 
     * @tparam T 对象类型
     * @tparam ObjectIdType ID类型
     */
    template<typename T, typename ObjectIdType>
    class Object : public Wt::Dbo::Dbo<T>
    {
        static_assert(std::is_base_of_v<IdType, ObjectIdType>);
        static_assert(!std::is_same_v<IdType, ObjectIdType>);

    public:
        using pointer = ObjectPtr<T>;
        using ObjectId = ObjectIdType;

        /**
         * @brief 获取对象ID
         */
        ObjectId getId() const 
        { 
            return ObjectId{ Wt::Dbo::Dbo<T>::self()->Wt::Dbo::template Dbo<T>::id() }; 
        }

        // 禁止直接使用 id()，必须使用 getId()
        typename Wt::Dbo::dbo_traits<T>::IdType id() const = delete;

    protected:
        template<typename>
        friend class ObjectPtr;

        // 从ObjectPtr获取原始dbo指针
        template<typename SomeObject>
        static Wt::Dbo::ptr<SomeObject> getDboPtr(const ObjectPtr<SomeObject>& ptr)
        {
            return ptr._obj;
        }
    };
} // namespace lms::db

