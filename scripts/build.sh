#!/bin/bash

# 项目构建脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# 加载环境配置
if [ -f "${PROJECT_ROOT}/.envrc" ]; then
    source "${PROJECT_ROOT}/.envrc"
fi

cd "${PROJECT_ROOT}"

# 创建构建目录
mkdir -p build
cd build

# 配置 CMake
echo "配置 CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# 编译
echo "编译项目..."
make -j$(nproc)

echo "✅ 构建完成！"
