cmake_minimum_required(VERSION 3.12)
project(LMS VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -Wall -Wextra)
    add_definitions(-DDEBUG)
else()
    add_compile_options(-O3 -DNDEBUG)
endif()

# 查找依赖
find_package(PkgConfig REQUIRED)

# Wt 框架
pkg_check_modules(WT REQUIRED wt)
include_directories(${WT_INCLUDE_DIRS})

# Boost
find_package(Boost REQUIRED COMPONENTS system filesystem program_options)

# FFmpeg
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVUTIL REQUIRED libavutil)

# TagLib
pkg_check_modules(TAGLIB REQUIRED taglib)

# SQLite3
find_package(SQLite3 REQUIRED)

# OpenSSL
find_package(OpenSSL REQUIRED)

# 其他依赖
find_package(PkgConfig REQUIRED)
pkg_check_modules(PUGIXML REQUIRED pugixml)
pkg_check_modules(CONFIGPP REQUIRED libconfig++)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${WT_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${AVCODEC_INCLUDE_DIRS}
    ${AVFORMAT_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
    ${TAGLIB_INCLUDE_DIRS}
    ${PUGIXML_INCLUDE_DIRS}
    ${CONFIGPP_INCLUDE_DIRS}
)

# 添加子目录
add_subdirectory(src)

# 安装配置
install(DIRECTORY conf/ DESTINATION etc/lms)
install(DIRECTORY approot/ DESTINATION share/lms/approot)
install(DIRECTORY docroot/ DESTINATION share/lms/docroot)

