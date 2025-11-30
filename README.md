# Lightweight Music Server (LMS)

LMS 是一款自托管的音乐流媒体服务器，通过 Web 界面访问和管理音乐收藏。

## 项目状态

**当前阶段**：阶段 5 - Scanner Service（100% 完成）

**已完成功能**：
- ✅ Core 库（工具类、配置、日志）
- ✅ 数据库基础架构（ORM、事务、迁移）
- ✅ Auth Service（认证服务、密码管理、令牌管理）
- ✅ Scanner Service（文件扫描、元数据提取、数据库更新）

## 技术栈

- **语言**: C++20
- **Web 框架**: Wt 4.12.1+ (Wt::Dbo ORM)
- **数据库**: SQLite3
- **配置解析**: libconfig++
- **构建系统**: CMake 3.20+

## 快速开始

### 环境要求

- WSL2 (Windows) 或 Linux
- CMake 3.20+
- C++20 编译器 (GCC 10+ 或 Clang 12+)
- Wt 框架 4.12.1+

### 构建项目

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 编译
make -j4

# 运行测试
./bin/test_database
./bin/test_auth
```

## 项目结构

```
src/libs/
├── core/          # 核心工具库（Path, String, Random, UUID, Config, Logger）
├── database/      # 数据库抽象层（IDb, Session, Transaction, ORM）
├── audio/         # 音频处理接口（IAudioFileInfo, ITagReader）
└── services/      # 服务层
    ├── auth/      # 认证服务（密码管理、令牌管理）
    └── scanner/   # 扫描服务（文件扫描、元数据提取）
```

## 核心功能

### Core 库
- 路径操作、字符串处理、随机数生成
- UUID v4 生成和解析
- 配置文件解析（libconfig++）
- 日志系统（多级别、多模块）

### 数据库层
- Wt::Dbo ORM 封装
- 事务管理（读写事务）
- 数据库迁移系统
- 数据对象：User, Track, Artist, Release, Medium, Directory, MediaLibrary, TrackArtistLink, TrackEmbeddedImage, Artwork, TrackList, TrackListEntry, TrackLyrics

### Auth Service
- 密码哈希和验证（bcrypt）
- 认证令牌管理
- 登录限流
- 多种认证后端支持（Internal, PAM, HTTP Headers）

### Scanner Service
- 文件系统扫描（递归目录遍历）
- 音频文件元数据提取与 TrackMetadataParser
- 自动创建并关联 Artist / Release / Medium / Directory
- 元数据解析增强：多艺术家拆分、日期/编号容错，可通过 `scanner.metadata.*` 配置自定义分隔符、白名单（`artists-to-not-split`）与回退策略
- TrackArtistLink 关联：支持多艺术家关联，包括常规艺术家（Artist）和专辑艺术家（ReleaseArtist）类型
- 嵌入式封面提取与持久化：支持 MP3 (ID3v2 APIC)、FLAC Picture、MP4 covr，并写入 `TrackEmbeddedImage` / `TrackEmbeddedImageLink` 表
- `.lmsignore` 目录忽略：任意目录放置 `.lmsignore` 文件即可跳过扫描
- 扫描步骤：删除缺失文件、检测重复文件、同步媒体库字段、清理孤立条目、数据库优化/压缩
- 数据库增量更新、错误收集与统计

### Artwork Service
- 图像库（Image Library）：提供 `IEncodedImage` 接口和 `EncodedImage` 实现
- Artwork 数据库对象：支持从 `TrackEmbeddedImage` 获取封面图像
- Artwork Service：支持从数据库读取嵌入式封面图像，集成图像缓存
- ImageCache：图像缓存系统，支持 LRU 风格的缓存管理，提高图像加载性能
- 默认图像支持：支持默认专辑封面和艺术家图像（SVG 格式）

## 在 Windows WSL 中运行与测试

1. **准备 WSL 环境**
   ```powershell
   wsl --install
   ```
2. **在 WSL 中构建项目**
   ```powershell
   wsl bash -c "cd /mnt/e/LightweightMusicServer && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)"
   ```
3. **运行核心测试**
   ```powershell
   wsl bash -c "cd /mnt/e/LightweightMusicServer/build && ./bin/test_database && ./bin/test_auth && ./bin/test_scanner"
   ```
4. **（可选）调试版构建**
   ```powershell
   wsl bash -c "cd /mnt/e/LightweightMusicServer && rm -rf build && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j$(nproc)"
   ```

## 开发文档

- [开发日志.md](开发日志.md) - 详细的开发过程记录
- [学习进度跟踪.md](学习进度跟踪.md) - 当前开发进度

