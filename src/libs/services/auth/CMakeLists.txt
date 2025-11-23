cmake_minimum_required(VERSION 3.20)

# Auth Service 库

# 创建 Auth Service 库
add_library(lmsauth STATIC
    impl/AuthServiceBase.cpp
    impl/LoginThrottler.cpp
    impl/PasswordServiceBase.cpp
    impl/internal/InternalPasswordService.cpp
    impl/AuthTokenService.cpp
    impl/PasswordService.cpp
    impl/AuthService.cpp
)

# 添加 AuthTokenService.hpp 到包含目录
target_include_directories(lmsauth PRIVATE
    impl
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

# 添加测试程序
add_executable(test_auth test/test_auth.cpp)
target_link_libraries(test_auth PRIVATE 
    lmsauth
    lmscore
    lmsdatabase
    ${Boost_LIBRARIES}
)
target_include_directories(test_auth PRIVATE
    include
    impl
)

