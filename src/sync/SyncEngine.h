// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

/**
 * SyncEngine (stub)
 *
 * Manages the sync lifecycle: push outbox → pull remote changes → update UI.
 * Currently only simulates sync for UI wiring.
 * Real Graph API integration is planned for M3/M5.
 */
class SyncEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SyncState syncState READ syncState NOTIFY syncStateChanged)
    Q_PROPERTY(QString syncStatus READ syncStatus NOTIFY syncStatusChanged)
    Q_PROPERTY(QString lastSyncText READ lastSyncText NOTIFY lastSyncTextChanged)

public:
    enum class SyncState {
        Idle,
        Syncing,
        Error
    };
    Q_ENUM(SyncState)

    explicit SyncEngine(QObject *parent = nullptr);

    SyncState syncState() const;
    QString syncStatus() const;
    QString lastSyncText() const;

    /** Call after config is loaded to set up the auto-sync timer. */
    void configure(bool autoSync, int intervalSeconds);

public slots:
    /** Trigger a manual or automatic sync cycle. */
    void syncNow();

signals:
    void syncStateChanged(SyncState state);
    void syncStatusChanged(const QString &status);
    void lastSyncTextChanged(const QString &text);
    void syncFinished(bool success);

private slots:
    void onAutoSyncTimer();
    void finishSimulatedSync();

private:
    void setState(SyncState state);

    SyncState m_state = SyncState::Idle;
    QString   m_syncStatus;
    QString   m_lastSyncText;
    QTimer    m_autoSyncTimer;
    QTimer    m_simulationTimer; // simulates async sync duration
};
