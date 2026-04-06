# todo-foru

A lightweight, cute Microsoft To Do client for Linux built with **Qt 6** and **QML**.

[![License: MIT](https://img.shields.io/badge/License-MIT-pink.svg)](LICENSE)

---

## ✨ Features (planned)

- 📋 View, create, edit and delete tasks and task lists
- 📅 Due dates, reminders and task importance
- 🏷️ Cross-device tag sync via Microsoft Graph `categories`
- 🔄 Auto-sync + manual Sync button (offline-first with outbox queue)
- 🖨️ Print / Export PDF via system print dialog
- 🎨 Cute themes: `light-cute` and `dark-cute`
- 🔐 Microsoft account login via OAuth2 + PKCE

---

## 🚧 Current Status

This is **Milestone 1 – skeleton**.  The app builds and runs, showing the
login and main-page scaffold with sample data.  Real Microsoft Graph API
calls are coming in subsequent milestones.

---

## 📦 Build Prerequisites

| Dependency | Minimum version | Notes |
|------------|-----------------|-------|
| **Qt 6**   | 6.2             | Modules: Core, Gui, Quick, Qml, Sql |
| **CMake**  | 3.22            | |
| **GCC / Clang** | C++17 support | |
| **SQLite** | (bundled with Qt Sql) | |

### Installing Qt 6 on Ubuntu / Debian

```bash
# Option A – from Ubuntu 22.04+ apt:
sudo apt install qt6-base-dev qt6-declarative-dev qt6-tools-dev cmake ninja-build

# Option B – Qt Online Installer (recommended for latest Qt):
# https://www.qt.io/download-open-source
# Select: Qt 6.x → Desktop (GCC 64-bit) → Qt Quick / Qt Sql
```

### Installing Qt 6 on Fedora / RHEL

```bash
sudo dnf install qt6-qtbase-devel qt6-qtdeclarative-devel cmake ninja-build
```

### Installing Qt 6 on Arch Linux

```bash
sudo pacman -S qt6-base qt6-declarative cmake ninja
```

---

## 🔧 Building

```bash
git clone https://github.com/lqx3089/todo-foru.git
cd todo-foru

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The compiled binary is at `build/todo-foru`.

---

## ▶ Running

```bash
./build/todo-foru
```

On first launch, a default `config.json` is written to
`~/.config/todo-foru/config.json`.  Copy `config.json.example` there and
fill in your Azure `client_id` to enable real login:

```bash
mkdir -p ~/.config/todo-foru
cp config.json.example ~/.config/todo-foru/config.json
# edit config.json and set auth.client_id
```

> **Azure App Registration**  
> 1. Go to <https://portal.azure.com> → Azure Active Directory → App registrations → New registration  
> 2. Platform: **Mobile and desktop applications**, redirect URI: `http://localhost:53682/callback`  
> 3. Copy the **Application (client) ID** into `config.json` → `auth.client_id`

---

## 📁 Project Structure

```
todo-foru/
├── CMakeLists.txt          # Root CMake build file
├── config.json.example     # Config template (copy to ~/.config/todo-foru/)
├── src/
│   ├── main.cpp            # Entry point
│   ├── app/
│   │   └── AppViewModel.*  # Central QML/C++ bridge singleton
│   ├── core/
│   │   ├── ConfigService.* # JSON config loader with ~ expansion
│   │   └── Logging.*       # Qt message handler (console + file)
│   ├── auth/
│   │   └── AuthService.*   # OAuth2 login stub (real PKCE coming in M2)
│   ├── sync/
│   │   └── SyncEngine.*    # Sync state machine (stub, M3/M5)
│   └── data/
│       ├── Database.*      # SQLite open + schema (task_lists/tasks/sync_state/outbox)
│       ├── TaskListsModel.* # QAbstractListModel for sidebar
│       └── TasksModel.*    # QAbstractListModel for task list
├── qml/
│   ├── main.qml            # Root Window
│   ├── pages/
│   │   ├── LoginPage.qml   # Login UI
│   │   └── MainPage.qml    # Three-column main layout
│   ├── components/
│   │   ├── Toolbar.qml     # Top bar with Sync button
│   │   ├── SyncStatusBar.qml
│   │   ├── CuteButton.qml
│   │   ├── CuteCard.qml
│   │   ├── TaskListDelegate.qml
│   │   └── TaskDelegate.qml
│   └── theme/
│       └── Theme.qml       # Design tokens singleton (light-cute / dark-cute)
├── assets/                 # Icons, fonts (future)
└── docs/
    └── architecture.md
```

---

## 🗄️ Database Schema

The SQLite database lives at `~/.local/share/todo-foru/todo.db` (configurable).

| Table        | Purpose |
|--------------|---------|
| `task_lists` | Task list metadata synced from Microsoft To Do |
| `tasks`      | Individual tasks |
| `sync_state` | Delta tokens and last-sync timestamps per list |
| `outbox`     | Offline mutation queue (create/update/delete) |

---

## ⚙️ Configuration Keys

See [`config.json.example`](config.json.example) for the full schema.

Key fields:

| Key | Description |
|-----|-------------|
| `auth.client_id` | **Required** – Azure app client ID |
| `auth.redirect_uri` | Must match Azure registration |
| `storage.db_path` | SQLite database path (supports `~`) |
| `sync.auto_sync_interval_seconds` | Auto-sync frequency (default 300 s) |
| `ui.theme` | `light-cute` or `dark-cute` |
| `logging.level` | `debug` / `info` / `warning` / `error` |
| `logging.file` | Log file path (supports `~`) |

---

## 🤝 Contributing

Pull requests are welcome!  Please:
- Follow the existing code style (C++17, Qt coding conventions)
- Keep Qt module dependencies minimal (Core/Gui/Quick/Qml/Sql)
- Use MIT-compatible assets only

---

## 📄 License

[MIT](LICENSE) – Copyright © 2026 XiQiu
