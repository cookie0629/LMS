#!/bin/bash

# 项目配置脚本
# 配置 PKG_CONFIG_PATH 并验证所有依赖

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
INSTALL_PREFIX="/usr/local"

echo "=========================================="
echo "LMS 项目配置"
echo "=========================================="

# 1. 配置 PKG_CONFIG_PATH
echo "步骤 1/4: 配置 PKG_CONFIG_PATH..."

WT_PKG_CONFIG_PATH="${INSTALL_PREFIX}/lib/pkgconfig"
if [ -d "${WT_PKG_CONFIG_PATH}" ]; then
    export PKG_CONFIG_PATH="${WT_PKG_CONFIG_PATH}:${PKG_CONFIG_PATH}"
    echo "✅ 已设置 PKG_CONFIG_PATH"
else
    echo "⚠️  警告: ${WT_PKG_CONFIG_PATH} 不存在"
fi

# 2. 验证 Wt 安装
echo ""
echo "步骤 2/4: 验证 Wt 安装..."
if pkg-config --exists wt 2>/dev/null; then
    WT_VERSION=$(pkg-config --modversion wt)
    echo "✅ Wt ${WT_VERSION} 已安装"
    
    # 显示 Wt 配置信息
    echo ""
    echo "Wt 配置信息:"
    pkg-config --cflags wt
    pkg-config --libs wt
else
    echo "❌ Wt 未找到"
    echo ""
    echo "请先运行:"
    echo "  bash scripts/complete-wt-setup.sh"
    exit 1
fi

# 3. 验证其他依赖
echo ""
echo "步骤 3/4: 验证其他依赖..."
MISSING_DEPS=0

check_dep() {
    if pkg-config --exists "$1" 2>/dev/null || \
       ldconfig -p | grep -q "$1" 2>/dev/null; then
        echo "✅ $1"
        return 0
    else
        echo "❌ $1"
        ((MISSING_DEPS++))
        return 1
    fi
}

check_dep "libavcodec"
check_dep "libavformat"
check_dep "taglib"
check_dep "libssl"
check_dep "libsqlite3"

if [ $MISSING_DEPS -gt 0 ]; then
    echo ""
    echo "⚠️  有 $MISSING_DEPS 个依赖缺失，但可以继续"
fi

# 4. 创建环境配置文件
echo ""
echo "步骤 4/4: 创建环境配置文件..."

ENV_FILE="${PROJECT_ROOT}/.envrc"
cat > "${ENV_FILE}" << EOF
# LMS 项目环境配置
# 使用 direnv 或手动 source 此文件

# Wt pkg-config 路径
export PKG_CONFIG_PATH="${WT_PKG_CONFIG_PATH}:\${PKG_CONFIG_PATH}"

# 库路径
export LD_LIBRARY_PATH="${INSTALL_PREFIX}/lib:\${LD_LIBRARY_PATH}"

# 显示配置
echo "PKG_CONFIG_PATH: \${PKG_CONFIG_PATH}"
echo "LD_LIBRARY_PATH: \${LD_LIBRARY_PATH}"
EOF

chmod +x "${ENV_FILE}"
echo "✅ 已创建 ${ENV_FILE}"

# 5. 创建构建脚本
echo ""
echo "创建构建辅助脚本..."

BUILD_SCRIPT="${PROJECT_ROOT}/scripts/build.sh"
cat > "${BUILD_SCRIPT}" << 'BUILD_EOF'
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
BUILD_EOF

chmod +x "${BUILD_SCRIPT}"
echo "✅ 已创建 ${BUILD_SCRIPT}"

echo ""
echo "=========================================="
echo "✅ 项目配置完成！"
echo "=========================================="
echo ""
echo "下一步："
echo "1. 加载环境配置:"
echo "   source .envrc"
echo ""
echo "2. 或者每次构建前运行:"
echo "   export PKG_CONFIG_PATH=\"${WT_PKG_CONFIG_PATH}:\${PKG_CONFIG_PATH}\""
echo ""
echo "3. 开始构建项目:"
echo "   bash scripts/build.sh"
echo "   或"
echo "   mkdir -p build && cd build"
echo "   cmake .. -DCMAKE_BUILD_TYPE=Debug"
echo "   make -j\$(nproc)"
echo ""

