# Git提交指南 - 渐进式提交模式

## 📋 目录
1. [提交规范](#提交规范)
2. [提交流程](#提交流程)
3. [提交信息模板](#提交信息模板)
4. [提交检查清单](#提交检查清单)

---

## 提交规范

### 提交信息格式

```
<type>: <subject>

<body>

<footer>
```

### 类型（type）说明

- **feat**: 新功能
- **fix**: 修复bug
- **docs**: 文档更新
- **test**: 测试相关
- **refactor**: 重构代码
- **chore**: 构建/工具相关
- **style**: 代码格式（不影响功能）

### 主题（subject）要求

- 简洁明了，不超过50字符
- 使用中文或英文
- 使用祈使语气（"实现"而不是"实现了"）

### 正文（body）要求

- 详细描述变更内容
- 每行一个要点
- 说明为什么这样改

### 页脚（footer）可选

- `Related: 相关模块`
- `Tests: 测试情况`
- `Closes: #issue编号`

---

## 提交流程

### 1. 开发前准备

```bash
# 确保在最新代码基础上
git pull

# 创建功能分支（可选，建议）
git checkout -b feature/path-utils
```

### 2. 开发功能

- 编写代码
- 测试功能
- 确保可以编译运行

### 3. 提交前检查

```bash
# 查看变更
git status
git diff

# 检查编译
cd build && make -j$(nproc)

# 运行测试（如果有）
./bin/test_path
```

### 4. 添加文件

```bash
# 添加特定文件
git add src/libs/core/include/core/Path.hpp
git add src/libs/core/impl/Path.cpp

# 或添加所有变更
git add .
```

### 5. 提交

```bash
git commit -m "feat: 实现Path工具类基础功能

- 创建 pathUtils 命名空间
- 实现 exists(), isDirectory(), isFile() 方法
- 使用 std::filesystem 实现跨平台支持

Related: 阶段1-Core库基础"
```

### 6. 推送（完成功能后）

```bash
# 推送到远程（如果使用分支）
git push origin feature/path-utils

# 或直接推送到main（如果直接在主分支开发）
git push origin main
```

---

## 提交信息模板

### 新功能提交

```
feat: [功能标题]

- 实现了 [具体功能1]
- 添加了 [具体功能2]
- 创建了 [文件列表]

Related: [相关模块]
Tests: [测试情况]
```

### Bug修复提交

```
fix: [问题描述]

- 修复了 [具体问题]
- 原因: [问题原因]
- 解决方案: [解决方法]

Related: [相关模块]
```

### 测试提交

```
test: [测试内容]

- 添加了 [测试用例]
- 覆盖了 [功能范围]
- 测试结果: [通过/失败]
```

### 文档提交

```
docs: [文档内容]

- 更新了 [文档名称]
- 添加了 [新章节]
- 修正了 [错误内容]
```

---

## 提交检查清单

### 代码质量

- [ ] 代码可以编译通过
- [ ] 功能可以正常运行
- [ ] 没有编译警告
- [ ] 代码格式正确（clang-format）

### 提交内容

- [ ] 只包含相关的变更
- [ ] 没有临时文件
- [ ] 没有调试代码
- [ ] 没有注释掉的代码

### 提交信息

- [ ] 提交信息清晰明确
- [ ] 说明了变更内容
- [ ] 说明了变更原因（如需要）
- [ ] 格式符合规范

### 测试验证

- [ ] 功能测试通过
- [ ] 边界情况测试（如需要）
- [ ] 跨平台测试（如需要）

---

## 提交频率建议

### 理想频率

- **每个独立功能**：1个提交
- **每个bug修复**：1个提交
- **每天**：2-3个有意义的提交
- **每周**：10-15个提交

### 提交粒度

**太小**（不推荐）：
```
fix: 修复拼写错误
fix: 添加分号
fix: 修改变量名
```

**太大**（不推荐）：
```
feat: 实现整个Core库
```

**合适**（推荐）：
```
feat: 实现Path工具类基础功能
feat: 添加Path工具类路径操作方法
feat: 添加Path工具类扩展功能
test: 添加Path工具类单元测试
```

---

## 提交历史示例

### 好的提交历史

```
* feat: 实现Path工具类基础功能
* feat: 添加Path工具类路径操作方法
* feat: 添加Path工具类扩展功能
* test: 添加Path工具类单元测试
* fix: 修复Windows路径分隔符问题
* feat: 实现String工具类基础功能
* feat: 实现UUID生成功能
```

### 不好的提交历史

```
* 更新
* 修复
* 添加功能
* 修改
```

---

## 实用命令

### 查看提交历史

```bash
# 简洁视图
git log --oneline

# 详细视图
git log

# 图形视图
git log --graph --oneline --all
```

### 修改提交

```bash
# 修改最后一次提交信息
git commit --amend -m "新的提交信息"

# 添加文件到最后一次提交
git add forgotten_file.cpp
git commit --amend --no-edit
```

### 撤销提交

```bash
# 撤销最后一次提交（保留更改）
git reset --soft HEAD~1

# 撤销最后一次提交（丢弃更改）
git reset --hard HEAD~1
```

---

## 分支策略（可选）

### 功能分支工作流

```bash
# 创建功能分支
git checkout -b feature/path-utils

# 开发并提交
git add .
git commit -m "feat: ..."

# 完成功能后合并
git checkout main
git merge feature/path-utils

# 删除功能分支
git branch -d feature/path-utils
```

### 直接在主分支开发（简单项目）

```bash
# 直接在主分支提交
git add .
git commit -m "feat: ..."
git push origin main
```

---

## 下一步

现在你可以开始第一个提交了！

**第一个提交任务**：
1. 实现 Path 工具类基础功能
2. 确保可以编译运行
3. 按照规范提交

参考 `移植路线图.md` 中的详细计划！

