cmake_minimum_required(VERSION 3.20)

# 创建 Database 库
add_library(lmsdatabase STATIC
    impl/Transaction.cpp
    impl/Object.cpp
    impl/Db.cpp
    impl/Session.cpp
)

target_include_directories(lmsdatabase INTERFACE
    include
)

target_include_directories(lmsdatabase PRIVATE
    impl
    include
)

# 链接依赖
target_link_libraries(lmsdatabase PUBLIC
    lmscore
    ${WT_LIBRARIES}
)

target_link_libraries(lmsdatabase PRIVATE
    ${SQLite3_LIBRARIES}
)

# 设置 C++20 标准
set_target_properties(lmsdatabase PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

