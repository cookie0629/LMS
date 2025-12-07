# Lightweight Music Server (LMS)

LMS 是一款自托管的音乐流媒体服务器，通过 Web 界面访问和管理音乐收藏。支持多种音频格式，提供完整的音乐库管理、播放队列、转码、播客和推荐等功能。

## 主要特性

- 🎵 **音乐库管理**：自动扫描音乐文件，提取元数据和封面
- 🎨 **Web 界面**：现代化的 Web 界面，支持多语言
- 🔐 **用户认证**：支持内部认证、PAM 和 HTTP Headers 认证
- 🎧 **音频转码**：支持多种音频格式的实时转码
- 📻 **播客支持**：播客订阅和管理功能
- 🎯 **智能推荐**：基于用户行为的音乐推荐
- 🔍 **Subsonic API**：兼容 Subsonic API，支持多种音乐客户端

## 技术栈

- **语言**: C++20
- **Web 框架**: Wt 4.12.1+ (Wt::Dbo ORM)
- **数据库**: SQLite3
- **配置解析**: libconfig++
- **构建系统**: CMake 3.20+

## 环境要求

- Linux 或 WSL2 (Windows)
- CMake 3.20+
- C++20 编译器 (GCC 10+ 或 Clang 12+)
- Wt 框架 4.12.1+
- 其他依赖：OpenSSL, Boost, TagLib, FFmpeg 等

## 构建项目

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
make -j$(nproc)

# 安装（可选）
sudo make install
```

### 调试构建

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## 运行

1. **配置**：复制 `conf/lms.conf.example` 为 `conf/lms.conf` 并修改配置
2. **启动**：运行编译后的可执行文件
3. **访问**：在浏览器中打开 `http://localhost:5082`

## 项目结构

```
src/
├── libs/
│   ├── core/          # 核心工具库（配置、日志、路径处理等）
│   ├── database/      # 数据库抽象层（ORM、事务、迁移）
│   ├── audio/         # 音频处理接口（元数据提取、标签读取）
│   ├── image/         # 图像处理（封面提取、缓存）
│   └── services/      # 服务层
│       ├── auth/      # 认证服务
│       ├── scanner/   # 扫描服务
│       ├── transcoding/ # 转码服务
│       ├── podcast/   # 播客服务
│       ├── recommendation/ # 推荐服务
│       └── artwork/   # 封面服务
└── lms/               # 主程序
    └── ui/            # Web 界面
```

