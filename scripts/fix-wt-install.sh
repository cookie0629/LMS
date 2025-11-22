#!/bin/bash

# 修复 Wt 安装问题的脚本
# 清理旧的构建并重新配置

set -e

BUILD_DIR="$HOME/wt-build"
WT_VERSION="4.12.1"

echo "=========================================="
echo "修复 Wt 安装"
echo "=========================================="

if [ ! -d "${BUILD_DIR}/wt-${WT_VERSION}" ]; then
    echo "错误: 未找到 Wt 源码目录"
    echo "请先运行: bash scripts/install-wt.sh"
    exit 1
fi

cd "${BUILD_DIR}/wt-${WT_VERSION}"

# 清理旧的构建目录
if [ -d "build" ]; then
    echo "清理旧的构建目录..."
    rm -rf build
fi

mkdir -p build
cd build

echo "重新配置 CMake（使用 STB 图像实现）..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DSHARED_LIBS=ON \
    -DWT_BUILD_EXAMPLES=OFF \
    -DWT_BUILD_TESTS=OFF \
    -DWT_WRASTERIMAGE_IMPLEMENTATION=Wt \
    -DWT_CPP_11_MODE=-std=c++11

echo ""
echo "配置完成！现在可以继续编译："
echo "  make -j\$(nproc)"
echo "  sudo make install"
echo ""

