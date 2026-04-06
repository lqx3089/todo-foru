# todo-foru

一个为 Linux 打造的轻量级、可爱的 Microsoft To Do 客户端，基于 **Qt 6** 和 **QML** 开发。

[![License: MIT](https://img.shields.io/badge/License-MIT-pink.svg)](LICENSE)

---

## ✨ 计划特性

- 📋 查看、创建、编辑和删除任务及任务列表
- 📅 支持截止日期、提醒和任务重要性
- 🏷️ 通过 Microsoft Graph `categories` 实现跨设备标签同步
- 🔄 自动同步 + 手动同步按钮（离线优先，带有外发队列）
- 🖨️ 通过系统打印对话框打印/导出 PDF
- 🎨 可爱主题：`light-cute` 和 `dark-cute`
- 🔐 通过 OAuth2 + PKCE 实现微软账号登录

---

## 🚧 当前状态

目前为 **里程碑 1 – 框架搭建**。应用已可编译运行，展示登录页和主页面架构（含示例数据）。后续版本将接入真实的 Microsoft Graph API。

---

## 📦 构建前置条件

| 依赖项 | 最低版本 | 说明 |
|--------|----------|------|
| **Qt 6** | 6.2 | 需包含 Core、Gui、Quick、Qml、Sql 模块 |
| **CMake** | 3.22 | |
| **GCC / Clang** | 支持 C++17 | |
| **SQLite** | （Qt Sql 已集成） | |

### Ubuntu / Debian 安装 Qt 6

```bash
# 方案 A – Ubuntu 22.04+ apt 安装：
sudo apt install qt6-base-dev qt6-declarative-dev qt6-tools-dev cmake ninja-build

# 方案 B – Qt 在线安装器（推荐最新 Qt）：
# https://www.qt.io/download-open-source
# 选择：Qt 6.x → Desktop (GCC 64-bit) → Qt Quick / Qt Sql
```

### Fedora / RHEL 安装 Qt 6

```bash
sudo dnf install qt6-qtbase-devel qt6-qtdeclarative-devel cmake ninja-build
```

### Arch Linux 安装 Qt 6

```bash
sudo pacman -S qt6-base qt6-declarative cmake ninja
```

---

## 🔧 构建方法

```bash
git clone https://github.com/lqx3089/todo-foru.git
cd todo-foru

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

编译后的可执行文件位于 `build/todo-foru`。

---

## ▶ 运行方法

```bash
./build/todo-foru
```

首次启动时，默认配置文件 `config.json` 会写入到 `~/.config/todo-foru/config.json`。
请将 `config.json.example` 复制到该目录，并填写你的 Azure `client_id` 以启用真实登录：

```bash
mkdir -p ~/.config/todo-foru
cp config.json.example ~/.config/todo-foru/config.json
# 编辑 config.json，设置 auth.client_id
```

> **Azure 应用注册**  
> 1. 访问 <https://portal.azure.com> → Azure Active Directory → 应用注册 → 新注册  
> 2. 平台选择：**移动和桌面应用**，重定向 URI：`http://localhost:53682/callback`  
> 3. 将 **应用程序 (客户端) ID** 填入 `config.json` → `auth.client_id`

---

## 📁 项目结构

```
todo-foru/
├── CMakeLists.txt          # 根 CMake 构建文件
├── config.json.example     # 配置模板（复制到 ~/.config/todo-foru/）
├── src/
│   ├── main.cpp            # 程序入口
│   ├── app/
│   │   └── AppViewModel.*  # QML/C++ 桥接单例
│   ├── core/
│   │   ├── ConfigService.* # JSON 配置加载，支持 ~ 展开
│   │   └── Logging.*       # Qt 消息处理（控制台+文件）
│   ├── auth/
│   │   └── AuthService.*   # OAuth2 登录（后续支持 PKCE）
│   ├── sync/
│   │   └── SyncEngine.*    # 同步状态机（后续开发）
│   └── data/
│       ├── Database.*      # SQLite 打开与建表（task_lists/tasks/sync_state/outbox）
│       ├── TaskListsModel.* # 侧边栏 QAbstractListModel
│       └── TasksModel.*    # 任务列表 QAbstractListModel
├── qml/
│   ├── main.qml            # 根窗口
│   ├── pages/
│   │   ├── LoginPage.qml   # 登录界面
│   │   └── MainPage.qml    # 三栏主界面
│   ├── components/
│   │   ├── Toolbar.qml     # 顶部工具栏（含同步按钮）
│   │   ├── SyncStatusBar.qml
│   │   ├── CuteButton.qml
│   │   ├── CuteCard.qml
│   │   ├── TaskListDelegate.qml
│   │   └── TaskDelegate.qml
│   └── theme/
│       └── Theme.qml       # 设计令牌单例（light-cute / dark-cute）
├── assets/                 # 图标、字体（后续）
└── docs/
    └── architecture.md
```

---

## 🗄️ 数据库结构

SQLite 数据库默认位于 `~/.local/share/todo-foru/todo.db`（可配置）。

| 表名         | 说明 |
|--------------|------|
| `task_lists` | 从 Microsoft To Do 同步的任务列表元数据 |
| `tasks`      | 具体任务 |
| `sync_state` | 每个列表的增量令牌和最后同步时间戳 |
| `outbox`     | 离线操作队列（增删改） |

---

## ⚙️ 配置项说明

完整配置项见 [`config.json.example`](config.json.example)。

主要字段：

| 键 | 说明 |
|----|------|
| `auth.client_id` | **必填** – Azure 应用客户端 ID |
| `auth.redirect_uri` | 需与 Azure 注册一致 |
| `storage.db_path` | SQLite 数据库路径（支持 `~`） |
| `sync.auto_sync_interval_seconds` | 自动同步频率（默认 300 秒） |
| `ui.theme` | `light-cute` 或 `dark-cute` |
| `logging.level` | `debug` / `info` / `warning` / `error` |
| `logging.file` | 日志文件路径（支持 `~`） |

---

## 🤝 贡献指南

欢迎 PR！请：
- 遵循现有代码风格（C++17，Qt 编码规范）
- 保持 Qt 模块依赖最小化（仅 Core/Gui/Quick/Qml/Sql）
- 仅使用 MIT 兼容的资源

---

## 📄 许可证

[MIT](LICENSE) – 版权所有 © 2026 XiQiu
