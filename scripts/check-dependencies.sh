#!/bin/bash

# 依赖检查脚本
# 检查所有必需的依赖是否已安装

set -e

echo "=========================================="
echo "LMS 项目依赖检查"
echo "=========================================="

ERRORS=0
WARNINGS=0

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

check_command() {
    if command -v "$1" &> /dev/null; then
        VERSION=$($1 --version 2>&1 | head -n 1)
        echo -e "${GREEN}✅${NC} $1: $VERSION"
        return 0
    else
        echo -e "${RED}❌${NC} $1: 未安装"
        ((ERRORS++))
        return 1
    fi
}

check_pkg_config() {
    if pkg-config --exists "$1" 2>/dev/null; then
        VERSION=$(pkg-config --modversion "$1")
        echo -e "${GREEN}✅${NC} $1: $VERSION"
        return 0
    else
        echo -e "${RED}❌${NC} $1: 未找到"
        ((ERRORS++))
        return 1
    fi
}

check_library() {
    if ldconfig -p | grep -q "$1"; then
        echo -e "${GREEN}✅${NC} $1: 已安装"
        return 0
    else
        echo -e "${YELLOW}⚠️${NC}  $1: 未在系统库缓存中找到（可能已安装但未更新缓存）"
        ((WARNINGS++))
        return 1
    fi
}

echo ""
echo "=== 基础工具 ==="
check_command "g++"
check_command "cmake"
check_command "git"
check_command "pkg-config"
check_command "make"

echo ""
echo "=== 开发工具 ==="
check_command "gdb" || echo -e "${YELLOW}⚠️${NC}  gdb: 可选（用于调试）"
check_command "clang-format" || echo -e "${YELLOW}⚠️${NC}  clang-format: 可选（用于代码格式化）"

echo ""
echo "=== 核心依赖库 ==="
check_pkg_config "libavcodec" || check_library "libavcodec"
check_pkg_config "libavformat" || check_library "libavformat"
check_pkg_config "libavutil" || check_library "libavutil"
check_pkg_config "taglib" || check_library "libtag"

echo ""
echo "=== Boost 库 ==="
if pkg-config --exists boost 2>/dev/null; then
    VERSION=$(pkg-config --modversion boost)
    echo -e "${GREEN}✅${NC} boost: $VERSION"
elif [ -f /usr/include/boost/version.hpp ]; then
    VERSION=$(grep "BOOST_LIB_VERSION" /usr/include/boost/version.hpp | head -1 | sed 's/.*"\(.*\)".*/\1/')
    echo -e "${GREEN}✅${NC} boost: $VERSION"
else
    echo -e "${RED}❌${NC} boost: 未找到"
    ((ERRORS++))
fi

echo ""
echo "=== Wt 框架 ==="
if pkg-config --exists wt 2>/dev/null; then
    VERSION=$(pkg-config --modversion wt)
    echo -e "${GREEN}✅${NC} wt: $VERSION"
elif [ -f /usr/local/lib/libwt.so ] || [ -f /usr/lib/libwt.so ]; then
    echo -e "${YELLOW}⚠️${NC}  wt: 库文件存在但 pkg-config 未找到"
    echo "   尝试: export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:\$PKG_CONFIG_PATH"
    ((WARNINGS++))
else
    echo -e "${RED}❌${NC} wt: 未安装（需要从源码编译）"
    echo "   运行: bash scripts/install-wt.sh"
    ((ERRORS++))
fi

echo ""
echo "=== 其他依赖库 ==="
check_library "libssl"
check_library "libsqlite3"
check_library "libpugixml"
check_library "libarchive"
check_library "libxxhash"

echo ""
echo "=========================================="
if [ $ERRORS -eq 0 ]; then
    if [ $WARNINGS -eq 0 ]; then
        echo -e "${GREEN}✅ 所有依赖检查通过！${NC}"
    else
        echo -e "${YELLOW}⚠️  有 $WARNINGS 个警告，但可以继续${NC}"
    fi
    echo "=========================================="
    exit 0
else
    echo -e "${RED}❌ 发现 $ERRORS 个错误，请先安装缺失的依赖${NC}"
    echo "=========================================="
    exit 1
fi

