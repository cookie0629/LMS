// 持久化对象基类定义

#pragma once

#include <Wt/Dbo/ptr.h>

#include "database/IdType.hpp"

namespace lms::db
{
    class ObjectPtrBase
    {
    protected:
        static void checkWriteTransaction(Wt::Dbo::Session& session);
    };

    template<typename T>
    class ObjectPtr : public ObjectPtrBase
    {
    public:
        ObjectPtr() = default;
        ObjectPtr(const Wt::Dbo::ptr<T>& obj)
            : _obj{ obj } {}
        ObjectPtr(Wt::Dbo::ptr<T>&& obj)
            : _obj{ std::move(obj) } {}

        const T* operator->() const { return _obj.get(); }
        operator bool() const { return _obj.get(); }
        bool operator!() const { return !_obj.get(); }
        bool operator==(const ObjectPtr& other) const { return _obj == other._obj; }
        bool operator!=(const ObjectPtr& other) const { return other._obj != _obj; }

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

    template<typename T, typename ObjectIdType>
    class Object : public Wt::Dbo::Dbo<T>
    {
        static_assert(std::is_base_of_v<db::IdType, ObjectIdType>);
        static_assert(!std::is_same_v<db::IdType, ObjectIdType>);

    public:
        using pointer = ObjectPtr<T>;
        using IdType = ObjectIdType;

        IdType getId() const { return Wt::Dbo::Dbo<T>::self()->Wt::Dbo::template Dbo<T>::id(); }

        // catch some misuses
        typename Wt::Dbo::dbo_traits<T>::IdType id() const = delete;

    protected:
        template<typename>
        friend class ObjectPtr;

        // Can get raw dbo ptr only from Objects
        template<typename SomeObject>
        static Wt::Dbo::ptr<SomeObject> getDboPtr(const ObjectPtr<SomeObject>& ptr)
        {
            return ptr._obj;
        }
    };
} // namespace lms::db
