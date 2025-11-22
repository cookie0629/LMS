LMS is a self-hosted music streaming software: access your music collection from anywhere using a web interface!
LMS 是一款自托管的音乐流媒体软件：通过网页界面随时访问您的音乐收藏！

## 📊 项目状态

**当前阶段**：阶段 3 - 数据库基础 ✅ 已完成

**已完成功能**：
- ✅ Core 库基础（Path, String, Random, UUID, Config, Logger）
- ✅ 数据库基础架构（IDb, Session, Transaction）
- ✅ 数据库对象系统（Object, IdType, User）
- ✅ 数据库迁移系统（VersionInfo, Migration）

**开发模式**：参考模板项目，完全手写实现，渐进式 Git 提交

## 🚀 快速开始

### 环境搭建

1. **Windows 用户**：使用 WSL2（推荐）
   ```powershell
   wsl
   cd /mnt/e/LightweightMusicServer
   bash scripts/setup-environment.sh
   ```

2. **Linux 用户**：直接运行安装脚本
   ```bash
   bash scripts/setup-environment.sh
   ```

### 构建项目

```bash
# 配置项目
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 编译
make -j4

# 运行测试
./bin/test_database
./bin/test_path
./bin/test_string
./bin/test_uuid
./bin/test_config
./bin/test_logger
```

### 学习资源

- 📖 [开发日志.md](开发日志.md) - 详细的开发过程记录
- 📖 [学习进度跟踪.md](学习进度跟踪.md) - 当前开发进度和任务
- 📚 [开发快速参考.md](开发快速参考.md) - 常用命令和代码模板
- 📚 [模板项目架构分析.md](模板项目架构分析.md) - 项目架构和技术栈分析

## 📋 项目结构

```
lms/
├── approot/          # Web UI 模板和资源
├── cmake/            # CMake 模块
├── conf/             # 配置文件
├── docroot/          # 静态 Web 资源
├── scripts/          # 构建和安装脚本
└── src/              # 源代码
    ├── libs/         # 核心库
    │   ├── core/     # Core 库（工具类、配置、日志）
    │   │   ├── include/core/    # 头文件
    │   │   ├── impl/            # 实现文件
    │   │   └── test/             # 测试程序
    │   └── database/ # Database 库（数据库抽象层）
    │       ├── include/database/ # 头文件
    │       ├── impl/             # 实现文件
    │       ├── impl/objects/     # 数据对象
    │       └── test/             # 测试程序
    ├── lms/          # 主程序
    └── tools/        # 工具程序
```

## 🛠️ 技术栈

- **语言**: C++20
- **Web 框架**: Wt 4.12.1+ (Wt::Dbo ORM)
- **数据库**: SQLite3
- **配置解析**: libconfig++
- **音频处理**: FFmpeg 4.0+, TagLib 2.1.1+ (计划中)
- **构建系统**: CMake 3.12+

## 📝 开发进度

### ✅ 已完成

**阶段 1：Core 库基础**
- Path 工具类（路径操作）
- String 工具类（字符串处理）
- Random 工具类（随机数生成）
- UUID 工具类（UUID v4 生成和解析）
- Config 配置系统（基于 libconfig++）
- Logger 日志系统（支持多级别、多模块）

**阶段 2：数据库基础**
- IDb 接口和 Db 实现（连接池、TLS 会话）
- Session 类（数据库会话管理）
- Transaction 类（读写事务）
- Object 基类（ORM 对象基类）
- IdType 强类型 ID 系统
- User 数据对象（简化版）
- 数据库迁移系统（版本管理）

### 🚧 进行中

- 测试程序完善
- 数据库迁移流程优化

### 📋 计划中

- 服务层实现
- 业务逻辑模块
- Web UI 开发
- 音频处理集成

## 📚 文档

- [开发日志.md](开发日志.md) - 详细的开发过程、问题和解决方案
- [学习进度跟踪.md](学习进度跟踪.md) - 当前任务和进度跟踪
- [开发快速参考.md](开发快速参考.md) - 常用命令、代码模板和最佳实践
- [Git提交指南.md](Git提交指南.md) - Git 提交规范和流程

## 🤝 贡献

本项目采用渐进式开发模式，每个功能独立提交，提交信息遵循规范格式。

## 📄 许可证

本项目参考原始 LMS 项目的 GPL-3.0 许可证。