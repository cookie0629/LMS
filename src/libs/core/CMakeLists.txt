# Core 库 CMakeLists.txt

# 创建 Core 库
add_library(lmscore
    impl/Path.cpp
    # 后续会添加更多源文件
    # impl/String.cpp
    # impl/UUID.cpp
)

# 设置包含目录
target_include_directories(lmscore PUBLIC
    include/
)

# 链接标准库
target_link_libraries(lmscore PUBLIC
    # std::filesystem 在 C++17 中，通常不需要显式链接
    # 但某些编译器可能需要
)

# 设置 C++ 标准
target_compile_features(lmscore PUBLIC cxx_std_20)

# 测试程序（可选）
option(ENABLE_CORE_TESTS "Enable Core library tests" ON)
if(ENABLE_CORE_TESTS)
    add_executable(test_path test/test_path.cpp)
    target_link_libraries(test_path PRIVATE lmscore)
    target_include_directories(test_path PRIVATE include/)
endif()

