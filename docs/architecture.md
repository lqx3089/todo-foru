# Architecture Overview – todo-foru

## Module Dependency Diagram

```
QML (UI layer)
  └── AppViewModel  (C++/QML bridge singleton)
        ├── AuthService   (OAuth2 stub → M2: real PKCE)
        ├── SyncEngine    (sync state machine → M3/M5: real Graph API)
        ├── TaskListsModel (QAbstractListModel)
        └── TasksModel     (QAbstractListModel)

AppViewModel / Services
  ├── ConfigService  (loads ~/.config/todo-foru/config.json)
  ├── Logging        (Qt message handler → console + file)
  └── Database       (SQLite via Qt Sql)
```

## Data Flow

```
User action in QML
  → AppViewModel invokable (login / logout / syncNow)
  → AuthService or SyncEngine
  → Local DB write (immediate, UI updates instantly)
  → Outbox queue entry
  → SyncEngine push to Graph API (on next sync)
  → Pull remote changes
  → Update models → QML re-renders
```

## SQLite Schema

### task_lists
| Column       | Type    | Notes                    |
|--------------|---------|--------------------------|
| id           | INTEGER | PK, autoincrement        |
| server_id    | TEXT    | Graph `todoTaskList.id`  |
| display_name | TEXT    |                          |
| is_default   | INTEGER | boolean                  |
| is_shared    | INTEGER | boolean                  |
| owner_id     | TEXT    |                          |
| etag         | TEXT    |                          |
| created_at   | TEXT    | ISO 8601                 |
| updated_at   | TEXT    | ISO 8601                 |
| deleted_at   | TEXT    | soft-delete              |

### tasks
| Column        | Type    | Notes                              |
|---------------|---------|------------------------------------|
| id            | INTEGER | PK                                 |
| server_id     | TEXT    | Graph `todoTask.id`                |
| list_local_id | INTEGER | FK → task_lists.id                 |
| title         | TEXT    |                                    |
| body          | TEXT    | notes / description                |
| status        | TEXT    | notStarted / inProgress / completed|
| importance    | TEXT    | normal / high / low                |
| categories    | TEXT    | JSON array ["tag1","tag2"]         |
| due_date      | TEXT    | ISO 8601 date                      |
| reminder_date | TEXT    | ISO 8601 datetime                  |
| completed_at  | TEXT    |                                    |
| etag          | TEXT    |                                    |
| created_at    | TEXT    |                                    |
| updated_at    | TEXT    |                                    |
| deleted_at    | TEXT    | soft-delete                        |

### sync_state
| Column         | Type    | Notes                            |
|----------------|---------|----------------------------------|
| id             | INTEGER | PK                               |
| list_server_id | TEXT    | UNIQUE                           |
| delta_token    | TEXT    | Graph delta query token          |
| last_sync_time | TEXT    | ISO 8601                         |

### outbox
| Column         | Type    | Notes                                        |
|----------------|---------|----------------------------------------------|
| id             | INTEGER | PK                                           |
| entity_type    | TEXT    | "task" or "list"                             |
| entity_local_id| INTEGER |                                              |
| op             | TEXT    | "create" / "update" / "delete"               |
| payload_json   | TEXT    | Fields to PATCH/POST                         |
| created_at     | TEXT    |                                              |
| retry_count    | INTEGER | default 0                                    |
| last_error     | TEXT    |                                              |
| status         | TEXT    | pending / in_flight / done / failed          |

## Milestone Plan

| # | Milestone | Key deliverables |
|---|-----------|-----------------|
| M1 | **Skeleton** (this PR) | CMake + QML scaffold, Config, Logging, SQLite schema, Auth/Sync stubs, AppViewModel |
| M2 | **Auth** | Real OAuth2+PKCE, browser open, localhost callback server, token cache |
| M3 | **Read** | GraphClient, pull task lists + tasks, populate DB, real models |
| M4 | **CRUD** | Create/edit/delete tasks, due date picker, reminder picker, categories/tags |
| M5 | **Sync** | Outbox push, delta pull, conflict resolution, auto-sync timer |
| M6 | **Polish** | Print/PDF, settings page, theme polish, notifications |
