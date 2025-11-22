#!/bin/bash

# 查找 Wt 实际安装位置的脚本

echo "=========================================="
echo "查找 Wt 安装位置"
echo "=========================================="

echo ""
echo "1. 查找 libwt 库文件:"
find /usr/local /usr -name "libwt.so*" -o -name "libwt.a" 2>/dev/null | head -10

echo ""
echo "2. 查找 Wt 头文件:"
find /usr/local /usr -type d -name "Wt" 2>/dev/null | head -5

echo ""
echo "3. 查找 wt.pc 文件:"
find /usr/local /usr -name "wt.pc" 2>/dev/null

echo ""
echo "4. 检查 /usr/local/lib/pkgconfig/:"
ls -la /usr/local/lib/pkgconfig/ 2>/dev/null || echo "目录不存在"

echo ""
echo "5. 检查 /usr/lib/pkgconfig/:"
ls -la /usr/lib/pkgconfig/ | grep -i wt || echo "未找到 Wt 相关文件"

echo ""
echo "6. 检查已安装的 Wt 文件:"
dpkg -L libwt-dev 2>/dev/null || echo "未通过 apt 安装"

echo ""
echo "7. 检查 CMake 能否找到 Wt:"
if command -v cmake &> /dev/null; then
    cmake --find-package -DNAME=Wt -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=COMPILE 2>&1 | head -5 || echo "CMake 无法找到"
fi

