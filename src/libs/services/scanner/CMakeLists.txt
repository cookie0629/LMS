cmake_minimum_required(VERSION 3.20)

# Scanner Service 库

# 创建 Scanner Service 库（简化版，暂时只包含基础文件）
add_library(lmsscanner STATIC
    impl/ScannerService.cpp
    impl/ScannerStats.cpp
)

target_include_directories(lmsscanner INTERFACE
    include
)

target_include_directories(lmsscanner PRIVATE
    include
    impl
)

# 链接依赖
target_link_libraries(lmsscanner PUBLIC
    lmscore
    lmsdatabase
    ${Boost_LIBRARIES}
)

target_link_libraries(lmsscanner PRIVATE
    ${SQLite3_LIBRARIES}
)

# 设置 C++20 标准
set_target_properties(lmsscanner PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

