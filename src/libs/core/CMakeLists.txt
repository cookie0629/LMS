# Core 库 CMakeLists.txt

# 创建 Core 库
add_library(lmscore
    impl/Path.cpp
    impl/String.cpp
    impl/Random.cpp
    impl/UUID.cpp
    impl/Config.cpp
    impl/Logger.cpp
)

# 设置包含目录
target_include_directories(lmscore PUBLIC
    include/
)

# 链接标准库和依赖
target_link_libraries(lmscore PUBLIC
    # std::filesystem 在 C++17 中，通常不需要显式链接
    # 但某些编译器可能需要
    ${CONFIGPP_LIBRARIES}
)

# 设置 C++ 标准
target_compile_features(lmscore PUBLIC cxx_std_20)

# 测试程序（可选）
option(ENABLE_CORE_TESTS "Enable Core library tests" ON)
if(ENABLE_CORE_TESTS)
    add_executable(test_path test/test_path.cpp)
    target_link_libraries(test_path PRIVATE lmscore)
    target_include_directories(test_path PRIVATE include/)
    
    add_executable(test_string test/test_string.cpp)
    target_link_libraries(test_string PRIVATE lmscore)
    target_include_directories(test_string PRIVATE include/)
    
    add_executable(test_uuid test/test_uuid.cpp)
    target_link_libraries(test_uuid PRIVATE lmscore)
    target_include_directories(test_uuid PRIVATE include/)
    
    add_executable(test_config test/test_config.cpp)
    target_link_libraries(test_config PRIVATE lmscore)
    target_include_directories(test_config PRIVATE include/)
    target_link_libraries(test_config PRIVATE ${CONFIGPP_LIBRARIES})
    
    add_executable(test_logger test/test_logger.cpp)
    target_link_libraries(test_logger PRIVATE lmscore)
    target_include_directories(test_logger PRIVATE include/)
endif()

