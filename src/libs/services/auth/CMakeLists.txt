cmake_minimum_required(VERSION 3.20)

# Auth Service 库

# 创建 Auth Service 库
# 注意：目前只创建库结构，实现文件待添加
add_library(lmsauth STATIC
    impl/AuthTokenService.cpp
    impl/PasswordService.cpp
    impl/AuthService.cpp
)

target_include_directories(lmsauth PUBLIC
    include
)

target_include_directories(lmsauth PRIVATE
    impl
    include
)

# 链接依赖
target_link_libraries(lmsauth PUBLIC
    lmscore
    lmsdatabase
    ${Boost_LIBRARIES}
)

target_link_libraries(lmsauth PRIVATE
    ${SQLite3_LIBRARIES}
)

# 设置 C++20 标准
set_target_properties(lmsauth PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

