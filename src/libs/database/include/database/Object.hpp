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
        T* operator->() { return const_cast<T*>(_obj.get()); }
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
            auto idValue = Wt::Dbo::Dbo<T>::self()->Wt::Dbo::template Dbo<T>::id();
            // 使用显式构造避免歧义：先转换为值类型，再显式构造
            typename ObjectId::ValueType value = static_cast<typename ObjectId::ValueType>(idValue);
            // 直接使用值构造 ObjectId
            // 由于 ObjectId 继承自 IdType，而 IdType 有 explicit IdType(ValueType) 构造函数
            // 我们通过显式调用 IdType 的构造函数来避免歧义
            // 注意：这里使用临时变量来避免列表初始化的歧义
            typename ObjectId::ValueType tempValue = value;
            // 使用显式构造函数调用，避免歧义
            return ObjectId(IdType::ValueType(tempValue));
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

