// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "Database.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

Database::Database(QObject *parent)
    : QObject(parent)
{}

Database::~Database()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool Database::open(const QString &dbPath)
{
    // Ensure the parent directory exists
    QDir().mkpath(QFileInfo(dbPath).absolutePath());

    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QLatin1String(k_connectionName));
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qCritical() << "[Database] Cannot open database:" << m_db.lastError().text()
                    << "| path:" << dbPath;
        return false;
    }

    qInfo() << "[Database] Opened database at" << dbPath;

    // Enable WAL for better concurrent access
    QSqlQuery pragmaQuery(m_db);
    pragmaQuery.exec(QStringLiteral("PRAGMA journal_mode=WAL;"));
    pragmaQuery.exec(QStringLiteral("PRAGMA foreign_keys=ON;"));

    return applySchema();
}

bool Database::isOpen() const
{
    return m_db.isValid() && m_db.isOpen();
}

QSqlDatabase Database::connection() const
{
    return m_db;
}

bool Database::applySchema()
{
    QSqlQuery q(m_db);

    // ── task_lists ──────────────────────────────────────────────────────────
    const QString createTaskLists = QStringLiteral(R"sql(
        CREATE TABLE IF NOT EXISTS task_lists (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            server_id       TEXT    UNIQUE,
            display_name    TEXT    NOT NULL,
            is_default      INTEGER NOT NULL DEFAULT 0,
            is_shared       INTEGER NOT NULL DEFAULT 0,
            owner_id        TEXT,
            etag            TEXT,
            created_at      TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
            updated_at      TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
            deleted_at      TEXT
        )
    )sql");

    // ── tasks ───────────────────────────────────────────────────────────────
    const QString createTasks = QStringLiteral(R"sql(
        CREATE TABLE IF NOT EXISTS tasks (
            id                  INTEGER PRIMARY KEY AUTOINCREMENT,
            server_id           TEXT    UNIQUE,
            list_local_id       INTEGER NOT NULL REFERENCES task_lists(id) ON DELETE CASCADE,
            title               TEXT    NOT NULL,
            body                TEXT,
            status              TEXT    NOT NULL DEFAULT 'notStarted',
            importance          TEXT    NOT NULL DEFAULT 'normal',
            categories          TEXT,
            due_date            TEXT,
            reminder_date       TEXT,
            completed_at        TEXT,
            etag                TEXT,
            created_at          TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
            updated_at          TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
            deleted_at          TEXT
        )
    )sql");

    // ── sync_state ──────────────────────────────────────────────────────────
    const QString createSyncState = QStringLiteral(R"sql(
        CREATE TABLE IF NOT EXISTS sync_state (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            list_server_id  TEXT    NOT NULL UNIQUE,
            delta_token     TEXT,
            last_sync_time  TEXT
        )
    )sql");

    // ── outbox ──────────────────────────────────────────────────────────────
    const QString createOutbox = QStringLiteral(R"sql(
        CREATE TABLE IF NOT EXISTS outbox (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            entity_type     TEXT    NOT NULL,
            entity_local_id INTEGER NOT NULL,
            op              TEXT    NOT NULL,
            payload_json    TEXT,
            created_at      TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
            retry_count     INTEGER NOT NULL DEFAULT 0,
            last_error      TEXT,
            status          TEXT    NOT NULL DEFAULT 'pending'
        )
    )sql");

    const QStringList statements = {
        createTaskLists,
        createTasks,
        createSyncState,
        createOutbox,
    };

    for (const QString &stmt : statements) {
        if (!q.exec(stmt)) {
            qCritical() << "[Database] Schema error:" << q.lastError().text();
            return false;
        }
    }

    qInfo() << "[Database] Schema applied successfully";
    return verifySchema();
}

bool Database::verifySchema() const
{
    static const QStringList expectedTables = {
        QStringLiteral("task_lists"),
        QStringLiteral("tasks"),
        QStringLiteral("sync_state"),
        QStringLiteral("outbox"),
    };

    const QStringList actualTables = m_db.tables();
    bool allPresent = true;
    for (const QString &table : expectedTables) {
        if (!actualTables.contains(table)) {
            qWarning() << "[Database] Missing table:" << table;
            allPresent = false;
        }
    }

    if (allPresent) {
        qInfo() << "[Database] Schema smoke-test passed – all tables present";
    }
    return allPresent;
}
