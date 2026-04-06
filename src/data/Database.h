// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>

/**
 * Database
 *
 * Opens (or creates) the SQLite database at the configured path and applies
 * the initial schema. Designed to be called once during app startup.
 *
 * Schema (tables):
 *   task_lists  – synced task list metadata
 *   tasks       – individual tasks
 *   sync_state  – delta tokens / last sync times per list
 *   outbox      – queued offline mutations awaiting push
 */
class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database() override;

    /**
     * Open the database at @p dbPath and apply schema migrations.
     * Returns true on success.
     */
    bool open(const QString &dbPath);

    /** Returns true if the database is open and ready. */
    bool isOpen() const;

    /** Returns the underlying QSqlDatabase connection. */
    QSqlDatabase connection() const;

    /**
     * Smoke-test: verify that all expected tables exist.
     * Logs a warning for any missing table.
     * Returns true if all tables are present.
     */
    bool verifySchema() const;

private:
    bool applySchema();

    QSqlDatabase m_db;
    static constexpr const char *k_connectionName = "todo-foru-main";
};
