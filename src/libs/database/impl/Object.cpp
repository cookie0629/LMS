#include "database/Object.hpp"

#include <Wt/Dbo/Session.h>

namespace lms::db
{
    void ObjectPtrBase::checkWriteTransaction(Wt::Dbo::Session& /*session*/)
    {
        // 检查是否有活动的写事务
        // 在简化版本中，我们暂时不实现严格的检查
        // 实际项目中可以使用 TransactionChecker
    }
} // namespace lms::db

