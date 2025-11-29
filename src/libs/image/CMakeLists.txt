cmake_minimum_required(VERSION 3.20)

# 创建 Image 库
add_library(lmsimage STATIC
    impl/EncodedImage.cpp
)

target_include_directories(lmsimage INTERFACE
    include
)

target_include_directories(lmsimage PRIVATE
    impl
    include
)

# 链接依赖
target_link_libraries(lmsimage PUBLIC
    lmscore
)

# 设置 C++20 标准
set_target_properties(lmsimage PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

