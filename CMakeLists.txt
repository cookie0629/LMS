cmake_minimum_required(VERSION 3.12)

project(lms VERSION 3.72.1)

# 添加自定义 CMake 模块的搜索路径
# 项目目录下的 cmake/modules/ 文件夹中的自定义 CMake 脚本可以被找到
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules/)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
# 只在 Linux（非 macOS） 上生效
# 添加链接器标志 --no-undefined，要求所有符号都必须在链接时解析
# 有助于发现未定义的引用错误
if (UNIX AND NOT APPLE)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
endif ()

# 定义了一个选项 ENABLE_TESTS，默认启用
# 如果启用测试：
# 包含 CTest 模块（CMake 的测试框架）
# 查找并包含 Google Test 框架
option(ENABLE_TESTS "Enable tests" ON)

if(ENABLE_TESTS)
	include(CTest)
	find_package(GTest REQUIRED)
endif()

# 依赖库查找
# OpenSSL：加密和 SSL/TLS 支持
# PkgConfig：用于查找其他库
# Threads：多线程支持
# Filesystem：C++17 文件系统库（可能来自 Boost 或标准库）
# Boost：
# program_options：命令行参数解析
# iostreams：扩展的 I/O 流
# Wt（Web Toolkit）：
# Wt：核心 Web 框架
# Dbo：数据库对象映射
# DboSqlite3：SQLite3 数据库后端
# HTTP：HTTP 客户端/服务器组件
# Pugixml：XML 解析库
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(Filesystem REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options iostreams)
find_package(Wt REQUIRED COMPONENTS Wt Dbo DboSqlite3 HTTP)
find_package(Pugixml CONFIG REQUIRED)

# 确保 Wt 库及其所有组件都被正确找到
# 如果缺少任何组件，会报致命错误停止构建
if (NOT Wt_FOUND)
	message(FATAL_ERROR "Wt package not found!")
endif ()
if (NOT TARGET Wt::Wt)
	message(FATAL_ERROR "Cannot find Wt::Wt!")
endif ()
if (NOT TARGET Wt::Dbo)
	message(FATAL_ERROR "Cannot find Wt::Dbo!")
endif ()
if (NOT TARGET Wt::DboSqlite3)
	message(FATAL_ERROR "Cannot find Wt::DboSqlite3!")
endif ()
if (NOT TARGET Wt::HTTP)
	message(FATAL_ERROR "Cannot find Wt::HTTP!")
endif ()

# 可选构建基准测试，默认关闭
# 如果启用，需要 Google Benchmark 库
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
if (BUILD_BENCHMARKS)
	find_package(benchmark REQUIRED)
	message(STATUS "Building benchmarks")
endif()

# 如果不是 Debug 构建模式（如 Release、RelWithDebInfo）
# 添加 -DNDEBUG 宏定义，禁用 assert 并可能启用优化
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNDEBUG)
endif()

# 添加 src/ 子目录，其中应该包含实际的源代码和构建目标
add_subdirectory(src)

install(DIRECTORY approot DESTINATION share/lms)
install(DIRECTORY docroot DESTINATION share/lms)
install(FILES conf/systemd/default.service DESTINATION share/lms)
install(FILES conf/pam/lms DESTINATION share/lms)
install(FILES conf/lms.conf DESTINATION share/lms)

