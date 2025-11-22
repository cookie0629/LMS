#!/bin/bash

# Wt 框架安装脚本
# 用于自动下载、编译和安装 Wt 4.12.1

set -e  # 遇到错误立即退出

WT_VERSION="4.12.1"
WT_URL="https://github.com/emweb/wt/archive/refs/tags/${WT_VERSION}.tar.gz"
INSTALL_PREFIX="/usr/local"
BUILD_DIR="$HOME/wt-build"

echo "=========================================="
echo "Wt Framework 安装脚本"
echo "版本: ${WT_VERSION}"
echo "=========================================="

# 检查是否已安装
if pkg-config --exists wt; then
    INSTALLED_VERSION=$(pkg-config --modversion wt)
    echo "检测到已安装的 Wt 版本: ${INSTALLED_VERSION}"
    read -p "是否要重新安装? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "跳过安装"
        exit 0
    fi
fi

# 检查依赖
echo "检查依赖..."
if ! command -v cmake &> /dev/null; then
    echo "错误: 未找到 cmake，请先安装"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "错误: 未找到 make，请先安装"
    exit 1
fi

# 创建临时目录
echo "创建构建目录: ${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# 下载源码
if [ ! -f "${WT_VERSION}.tar.gz" ]; then
    echo "下载 Wt ${WT_VERSION} 源码..."
    wget "${WT_URL}" -O "${WT_VERSION}.tar.gz"
else
    echo "使用已下载的源码包"
fi

# 解压
if [ ! -d "wt-${WT_VERSION}" ]; then
    echo "解压源码..."
    tar -xzf "${WT_VERSION}.tar.gz"
fi

cd "wt-${WT_VERSION}"

# 创建构建目录
BUILD_SUBDIR="build"
if [ -d "${BUILD_SUBDIR}" ]; then
    echo "清理旧的构建目录..."
    rm -rf "${BUILD_SUBDIR}"
fi
mkdir -p "${BUILD_SUBDIR}"
cd "${BUILD_SUBDIR}"

# 配置 CMake
echo "配置 CMake..."
# 使用 STB 作为图像实现（不需要额外依赖）
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
    -DSHARED_LIBS=ON \
    -DWT_BUILD_EXAMPLES=OFF \
    -DWT_BUILD_TESTS=OFF \
    -DWT_WRASTERIMAGE_IMPLEMENTATION=Wt \
    -DWT_CPP_11_MODE=-std=c++11

# 编译
echo "开始编译 Wt（这可能需要 10-30 分钟，请耐心等待）..."
CPU_COUNT=$(nproc)
echo "使用 ${CPU_COUNT} 个 CPU 核心进行编译"
make -j${CPU_COUNT}

# 安装
echo "安装 Wt 到 ${INSTALL_PREFIX}..."
sudo make install

# 更新库缓存
echo "更新库缓存..."
sudo ldconfig

# 验证安装
echo "验证安装..."
if pkg-config --exists wt; then
    INSTALLED_VERSION=$(pkg-config --modversion wt)
    echo "✅ Wt ${INSTALLED_VERSION} 安装成功！"
    
    # 检查库文件
    echo "检查库文件..."
    ls -lh "${INSTALL_PREFIX}/lib/libwt"* | head -3
    
    # 检查头文件
    echo "检查头文件..."
    ls -d "${INSTALL_PREFIX}/include/Wt" 2>/dev/null && echo "✅ 头文件已安装"
    
else
    echo "⚠️  警告: pkg-config 找不到 Wt，可能需要设置 PKG_CONFIG_PATH"
    echo "尝试设置: export PKG_CONFIG_PATH=${INSTALL_PREFIX}/lib/pkgconfig:\$PKG_CONFIG_PATH"
fi

echo "=========================================="
echo "安装完成！"
echo "=========================================="

