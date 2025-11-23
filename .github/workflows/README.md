# GitHub Actions CI/CD

本目录包含项目的 CI/CD 工作流配置。

## 工作流

### CI (ci.yml)

**触发条件**：
- Push 到 `main` 或 `develop` 分支
- 创建 Pull Request 到 `main` 或 `develop` 分支

**功能**：
1. 在 Ubuntu 上构建项目（Debug 和 Release 模式）
2. 运行所有测试程序
3. 上传构建工件（仅 Release 模式）
4. 上传编译命令数据库（compile_commands.json）

**构建矩阵**：
- OS: Ubuntu Latest
- Build Type: Debug, Release

**依赖安装**：
- 自动安装所有必需的开发库
- 从源码编译安装 Wt 框架（如果系统未提供）

**测试**：
- 自动运行所有 `test_*` 测试程序
- 测试失败会导致 CI 失败

**工件**：
- Release 构建的可执行文件和库文件
- 编译命令数据库（用于 IDE 支持）

## 本地测试

在本地测试 CI 配置：

```bash
# 使用 act (GitHub Actions 本地运行器)
act -j build-and-test

# 或者手动运行 CI 步骤
bash scripts/setup-environment.sh
bash scripts/build.sh
cd build && make test
```

## 注意事项

1. **Wt 框架安装**：Wt 框架需要从源码编译，可能需要 10-20 分钟
2. **测试时间**：完整测试套件可能需要几分钟
3. **工件保留**：构建工件保留 7 天
4. **缓存**：考虑添加依赖缓存以加速后续构建

