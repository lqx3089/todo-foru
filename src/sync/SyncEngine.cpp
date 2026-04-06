// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "SyncEngine.h"

#include <QDateTime>
#include <QDebug>

SyncEngine::SyncEngine(QObject *parent)
    : QObject(parent)
    , m_syncStatus(QStringLiteral("Idle"))
    , m_lastSyncText(QStringLiteral("Never"))
{
    m_simulationTimer.setSingleShot(true);
    connect(&m_simulationTimer, &QTimer::timeout, this, &SyncEngine::finishSimulatedSync);
    connect(&m_autoSyncTimer,   &QTimer::timeout, this, &SyncEngine::onAutoSyncTimer);
}

SyncEngine::SyncState SyncEngine::syncState() const
{
    return m_state;
}

QString SyncEngine::syncStatus() const
{
    return m_syncStatus;
}

QString SyncEngine::lastSyncText() const
{
    return m_lastSyncText;
}

void SyncEngine::configure(bool autoSync, int intervalSeconds)
{
    m_autoSyncTimer.stop();
    if (autoSync && intervalSeconds > 0) {
        m_autoSyncTimer.start(intervalSeconds * 1000);
        qInfo() << "[SyncEngine] Auto-sync enabled, interval:" << intervalSeconds << "s";
    }
}

void SyncEngine::syncNow()
{
    if (m_state == SyncState::Syncing) {
        qInfo() << "[SyncEngine] Already syncing, ignoring syncNow()";
        return;
    }

    qInfo() << "[SyncEngine] syncNow() called (stub – real Graph API planned for M3/M5)";
    setState(SyncState::Syncing);

    // TODO M3: pushOutbox() → pullRemoteChanges() → update models
    // For now, simulate with a short delay
    m_simulationTimer.start(1500);
}

void SyncEngine::onAutoSyncTimer()
{
    qInfo() << "[SyncEngine] Auto-sync triggered";
    syncNow();
}

void SyncEngine::finishSimulatedSync()
{
    const QString now = QDateTime::currentDateTime()
                            .toString(QStringLiteral("HH:mm:ss"));
    m_lastSyncText = QStringLiteral("Last sync: %1").arg(now);
    emit lastSyncTextChanged(m_lastSyncText);

    setState(SyncState::Idle);
    emit syncFinished(true);
    qInfo() << "[SyncEngine] Simulated sync complete at" << now;
}

void SyncEngine::setState(SyncState state)
{
    if (m_state == state) return;
    m_state = state;

    switch (state) {
    case SyncState::Idle:    m_syncStatus = QStringLiteral("Idle");    break;
    case SyncState::Syncing: m_syncStatus = QStringLiteral("Syncing"); break;
    case SyncState::Error:   m_syncStatus = QStringLiteral("Error");   break;
    }

    emit syncStateChanged(m_state);
    emit syncStatusChanged(m_syncStatus);
}
