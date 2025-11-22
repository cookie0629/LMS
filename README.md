LMS is a self-hosted music streaming software: access your music collection from anywhere using a web interface!
LMS 是一款自托管的音乐流媒体软件：通过网页界面随时访问您的音乐收藏！

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

详细说明请查看：
- 📖 [快速开始.md](快速开始.md) - 快速入门指南
- 📖 [环境搭建指南.md](环境搭建指南.md) - 详细的环境搭建说明
- 📖 [详细开发步骤指南.md](详细开发步骤指南.md) - 完整的开发流程

### 学习资源

- 📚 [项目学习指南.md](项目学习指南.md) - 项目架构和技术栈分析
- 📚 [学习路径总结.md](学习路径总结.md) - 循序渐进的学习计划
- 📚 [开发快速参考.md](开发快速参考.md) - 常用命令和代码模板

## 📋 项目结构

```
lms/
├── approot/          # Web UI 模板和资源
├── cmake/            # CMake 模块
├── conf/             # 配置文件
├── docroot/          # 静态 Web 资源
└── src/              # 源代码
    ├── libs/         # 核心库
    ├── lms/          # 主程序
    └── tools/        # 工具程序
```

## 🛠️ 技术栈

- **语言**: C++20
- **Web 框架**: Wt 4.12.1+
- **数据库**: SQLite3 (通过 Wt::Dbo)
- **音频处理**: FFmpeg 4.0+, TagLib 2.1.1+
- **构建系统**: CMake 3.12+

## 📝 开发

详细开发指南请参考 [详细开发步骤指南.md](详细开发步骤指南.md)