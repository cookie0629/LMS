cmake_minimum_required(VERSION 3.20)

# 创建 Artwork Service 库
add_library(lmsartwork STATIC
    impl/ArtworkService.cpp
)

target_include_directories(lmsartwork INTERFACE
    include
)

target_include_directories(lmsartwork PRIVATE
    impl
    include
)

# 链接依赖
target_link_libraries(lmsartwork PUBLIC
    lmscore
    lmsdatabase
    lmsimage
    ${WT_LIBRARIES}
)

# 设置 C++20 标准
set_target_properties(lmsartwork PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

