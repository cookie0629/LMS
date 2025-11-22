#!/bin/bash

# 修复 Wt pkg-config 配置
# 如果 wt.pc 不存在，尝试创建或找到正确的路径

set -e

INSTALL_PREFIX="/usr/local"
WT_PKG_CONFIG_DIR="${INSTALL_PREFIX}/lib/pkgconfig"

echo "=========================================="
echo "修复 Wt pkg-config 配置"
echo "=========================================="

# 1. 查找 Wt 库文件
echo "步骤 1: 查找 Wt 库文件..."
WT_LIB=$(find /usr/local /usr -name "libwt.so*" -o -name "libwt.a" 2>/dev/null | head -1)

if [ -z "$WT_LIB" ]; then
    echo "❌ 未找到 Wt 库文件"
    echo "请先完成 Wt 的编译和安装"
    exit 1
fi

WT_LIB_DIR=$(dirname "$WT_LIB")
echo "✅ 找到 Wt 库: $WT_LIB"
echo "   库目录: $WT_LIB_DIR"

# 2. 查找 Wt 头文件
echo ""
echo "步骤 2: 查找 Wt 头文件..."
WT_INCLUDE=$(find /usr/local /usr -type d -name "Wt" -path "*/include/*" 2>/dev/null | head -1)

if [ -z "$WT_INCLUDE" ]; then
    echo "❌ 未找到 Wt 头文件目录"
    exit 1
fi

WT_INCLUDE_DIR=$(dirname "$WT_INCLUDE")
echo "✅ 找到 Wt 头文件: $WT_INCLUDE/WApplication.h"
echo "   包含目录: $WT_INCLUDE_DIR"

# 3. 检查或创建 pkg-config 目录
echo ""
echo "步骤 3: 检查 pkg-config 目录..."
if [ ! -d "$WT_PKG_CONFIG_DIR" ]; then
    echo "创建 pkg-config 目录: $WT_PKG_CONFIG_DIR"
    sudo mkdir -p "$WT_PKG_CONFIG_DIR"
fi

# 4. 检查是否已有 wt.pc
echo ""
echo "步骤 4: 检查 wt.pc 文件..."
if [ -f "$WT_PKG_CONFIG_DIR/wt.pc" ]; then
    echo "✅ wt.pc 已存在: $WT_PKG_CONFIG_DIR/wt.pc"
    cat "$WT_PKG_CONFIG_DIR/wt.pc"
else
    echo "创建 wt.pc 文件..."
    
    # 获取 Wt 版本（尝试多种方法）
    WT_VERSION="4.12.1"
    if [ -f "$WT_INCLUDE/Wt/WConfig.h" ]; then
        # 尝试从头文件读取版本
        VERSION_MAJOR=$(grep -oP 'WT_VERSION_MAJOR\s+\K\d+' "$WT_INCLUDE/Wt/WConfig.h" 2>/dev/null || echo "4")
        VERSION_MINOR=$(grep -oP 'WT_VERSION_MINOR\s+\K\d+' "$WT_INCLUDE/Wt/WConfig.h" 2>/dev/null || echo "12")
        VERSION_PATCH=$(grep -oP 'WT_VERSION_PATCH\s+\K\d+' "$WT_INCLUDE/Wt/WConfig.h" 2>/dev/null || echo "1")
        WT_VERSION="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
    fi
    
    # 创建 wt.pc 文件
    sudo tee "$WT_PKG_CONFIG_DIR/wt.pc" > /dev/null << EOF
prefix=${INSTALL_PREFIX}
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: Wt
Description: C++ Web Toolkit
Version: ${WT_VERSION}
Requires: 
Libs: -L\${libdir} -lwt -lwthttp -lwtdbo -lwtdbosqlite3
Cflags: -I\${includedir} -std=c++11
EOF
    
    echo "✅ 已创建 wt.pc"
    cat "$WT_PKG_CONFIG_DIR/wt.pc"
fi

# 5. 更新库缓存
echo ""
echo "步骤 5: 更新库缓存..."
sudo ldconfig

# 6. 设置 PKG_CONFIG_PATH
echo ""
echo "步骤 6: 配置 PKG_CONFIG_PATH..."
export PKG_CONFIG_PATH="$WT_PKG_CONFIG_DIR:$PKG_CONFIG_PATH"

# 添加到 ~/.bashrc
if ! grep -q "PKG_CONFIG_PATH.*$WT_PKG_CONFIG_DIR" ~/.bashrc 2>/dev/null; then
    echo "" >> ~/.bashrc
    echo "# Wt pkg-config path" >> ~/.bashrc
    echo "export PKG_CONFIG_PATH=\"$WT_PKG_CONFIG_DIR:\${PKG_CONFIG_PATH}\"" >> ~/.bashrc
    echo "✅ 已添加到 ~/.bashrc"
fi

# 7. 验证
echo ""
echo "步骤 7: 验证配置..."
if pkg-config --exists wt 2>/dev/null; then
    VERSION=$(pkg-config --modversion wt)
    echo "✅ 成功！Wt ${VERSION} 现在可以通过 pkg-config 找到"
    echo ""
    echo "配置信息:"
    pkg-config --cflags wt
    pkg-config --libs wt
else
    echo "❌ 仍然无法找到，请手动设置:"
    echo "   export PKG_CONFIG_PATH=\"$WT_PKG_CONFIG_DIR:\${PKG_CONFIG_PATH}\""
    exit 1
fi

echo ""
echo "=========================================="
echo "✅ 配置完成！"
echo "=========================================="

