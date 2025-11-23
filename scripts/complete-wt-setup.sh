#!/bin/bash

# 完成 Wt 安装和配置的脚本
# 解决 pkg-config 路径问题

set -e

BUILD_DIR="$HOME/wt-build"
WT_VERSION="4.12.1"
INSTALL_PREFIX="/usr/local"

echo "=========================================="
echo "完成 Wt 安装和配置"
echo "=========================================="

# 检查 Wt 是否已编译
if [ ! -f "${BUILD_DIR}/wt-${WT_VERSION}/build/src/libwt.so" ] && \
   [ ! -f "${BUILD_DIR}/wt-${WT_VERSION}/build/src/libwt.a" ]; then
    echo "Wt 尚未编译，开始编译..."
    cd "${BUILD_DIR}/wt-${WT_VERSION}/build"
    
    if [ ! -f "CMakeCache.txt" ]; then
        echo "重新配置 CMake..."
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
            -DSHARED_LIBS=ON \
            -DWT_BUILD_EXAMPLES=OFF \
            -DWT_BUILD_TESTS=OFF \
            -DWT_WRASTERIMAGE_IMPLEMENTATION=Wt \
            -DWT_CPP_11_MODE=-std=c++11
    fi
    
    echo "编译 Wt（这可能需要 10-30 分钟）..."
    make -j$(nproc)
else
    echo "Wt 已编译"
fi

# 检查是否已安装
if [ ! -f "${INSTALL_PREFIX}/lib/libwt.so" ] && \
   [ ! -f "${INSTALL_PREFIX}/lib/libwt.a" ]; then
    echo "安装 Wt..."
    cd "${BUILD_DIR}/wt-${WT_VERSION}/build"
    sudo make install
else
    echo "Wt 已安装"
fi

# 更新库缓存
echo "更新库缓存..."
sudo ldconfig

# 配置 PKG_CONFIG_PATH
echo "配置 PKG_CONFIG_PATH..."
WT_PKG_CONFIG_PATH="${INSTALL_PREFIX}/lib/pkgconfig"
if [ -f "${WT_PKG_CONFIG_PATH}/wt.pc" ]; then
    echo "找到 wt.pc 文件: ${WT_PKG_CONFIG_PATH}/wt.pc"
    
    # 添加到当前 shell 的 PKG_CONFIG_PATH
    export PKG_CONFIG_PATH="${WT_PKG_CONFIG_PATH}:${PKG_CONFIG_PATH}"
    
    # 添加到 ~/.bashrc（如果还没有）
    if ! grep -q "PKG_CONFIG_PATH.*wt" ~/.bashrc 2>/dev/null; then
        echo "" >> ~/.bashrc
        echo "# Wt pkg-config path" >> ~/.bashrc
        echo "export PKG_CONFIG_PATH=\"${WT_PKG_CONFIG_PATH}:\${PKG_CONFIG_PATH}\"" >> ~/.bashrc
        echo "已添加到 ~/.bashrc"
    fi
else
    echo "警告: 未找到 wt.pc 文件"
    echo "尝试查找..."
    find "${INSTALL_PREFIX}" -name "wt.pc" 2>/dev/null || echo "未找到 wt.pc"
fi

# 验证安装
echo ""
echo "验证安装..."
if pkg-config --exists wt 2>/dev/null; then
    VERSION=$(pkg-config --modversion wt)
    echo " Wt ${VERSION} 已正确配置！"
    echo ""
    echo "库文件位置:"
    ls -lh "${INSTALL_PREFIX}/lib/libwt"* 2>/dev/null | head -3 || echo "未找到库文件"
    echo ""
    echo "头文件位置:"
    ls -d "${INSTALL_PREFIX}/include/Wt" 2>/dev/null && echo " 头文件存在" || echo "未找到头文件"
else
    echo "  pkg-config 仍无法找到 Wt"
    echo ""
    echo "请手动设置:"
    echo "  export PKG_CONFIG_PATH=\"${WT_PKG_CONFIG_PATH}:\${PKG_CONFIG_PATH}\""
    echo ""
    echo "或者检查安装:"
    echo "  ls -la ${INSTALL_PREFIX}/lib/pkgconfig/"
fi

echo ""
echo "=========================================="
echo "配置完成！"
echo "=========================================="
echo ""
echo "如果 pkg-config 仍无法找到 Wt，请运行:"
echo "  export PKG_CONFIG_PATH=\"${WT_PKG_CONFIG_PATH}:\${PKG_CONFIG_PATH}\""
echo "  source ~/.bashrc"
echo ""

