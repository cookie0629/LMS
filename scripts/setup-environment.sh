#!/bin/bash

# LMS 项目环境一键搭建脚本
# 自动安装所有依赖和工具

set -e

echo "=========================================="
echo "LMS 项目环境搭建脚本"
echo "=========================================="
echo ""

# 检查是否在 WSL 或 Linux 环境中
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "错误: 此脚本需要在 Linux 或 WSL2 环境中运行"
    echo "请在 WSL2 中运行: wsl"
    exit 1
fi

# 检查是否为 root（不应该以 root 运行）
if [ "$EUID" -eq 0 ]; then
    echo "警告: 不建议以 root 用户运行此脚本"
    read -p "是否继续? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# 步骤 1: 更新系统
echo "=========================================="
echo "步骤 1/5: 更新系统包"
echo "=========================================="
sudo apt update
sudo apt upgrade -y

# 步骤 2: 安装基础工具
echo ""
echo "=========================================="
echo "步骤 2/5: 安装基础开发工具"
echo "=========================================="
sudo apt install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    ninja-build \
    software-properties-common \
    g++-12 \
    gcc-12 \
    clang-14 \
    clang-format \
    clang-tidy \
    gdb \
    valgrind

# 设置默认编译器
echo "设置默认编译器..."
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 2>/dev/null || true
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100 2>/dev/null || true

# 步骤 3: 安装依赖库
echo ""
echo "=========================================="
echo "步骤 3/5: 安装依赖库"
echo "=========================================="
sudo apt install -y \
    libboost-all-dev \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libavfilter-dev \
    libswscale-dev \
    libswresample-dev \
    ffmpeg \
    libtag1-dev \
    libstb-dev \
    libconfig++-dev \
    libpam0g-dev \
    libpugixml-dev \
    libgtest-dev \
    libarchive-dev \
    libxxhash-dev \
    libssl-dev \
    libsqlite3-dev \
    zlib1g-dev \
    libbz2-dev

# 步骤 4: 安装 Wt 框架
echo ""
echo "=========================================="
echo "步骤 4/5: 安装 Wt 框架"
echo "=========================================="
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [ -f "${SCRIPT_DIR}/install-wt.sh" ]; then
    bash "${SCRIPT_DIR}/install-wt.sh"
else
    echo "警告: 未找到 install-wt.sh，跳过 Wt 安装"
    echo "请手动运行: bash scripts/install-wt.sh"
fi

# 步骤 5: 验证安装
echo ""
echo "=========================================="
echo "步骤 5/5: 验证安装"
echo "=========================================="
if [ -f "${SCRIPT_DIR}/check-dependencies.sh" ]; then
    bash "${SCRIPT_DIR}/check-dependencies.sh"
else
    echo "检查关键工具..."
    g++ --version
    cmake --version
    pkg-config --modversion libavcodec || echo "警告: libavcodec 未找到"
fi

echo ""
echo "=========================================="
echo "✅ 环境搭建完成！"
echo "=========================================="
echo ""
echo "下一步："
echo "1. 进入项目目录: cd /mnt/e/LightweightMusicServer"
echo "2. 创建构建目录: mkdir -p build && cd build"
echo "3. 配置 CMake: cmake .. -DCMAKE_BUILD_TYPE=Debug"
echo "4. 编译项目: make -j\$(nproc)"
echo ""

